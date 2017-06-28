#include <iostream>
#include <string>
#include <sstream>
#include <opencv2/core.hpp> 						//opencv2 library
#include <opencv2/imgproc/imgproc.hpp>  //opencv2 library
#include <opencv2/highgui.hpp> 					//opencv2 library
#include <bitset> // For converting integer to binary number
#include <cmath>
#include <stdint.h> // For storing value from FPGA (16 bit data)
#include <unistd.h> // function sleep(second) and usleep(microsecond)

// DEFINE NAME
// ========================

#define UP    		"10"
#define BOTTOM 		"01"
#define LEFT  		"10"
#define RIGHT 		"01"
#define IDLE 	  	"00"
#define MAX_SPEED 255
#define IM_WIDTH  640
#define IM_HEIGHT 480

using namespace cv;
using namespace std;

// PID Control Signal Structure
struct pid {
	float Kp;		// The value for Proportional gain
	float Ki;		// The value for Integral gain
	float Kd;	// The value for Differential gain
	int P_Term;			// Holds the calculated Proportional value
	int I_Term;			// Holds the calculated Integral value
	int D_Term;			// Holds the calculated Differential value
	int p_Temp;			// Holds the temporary Proportional value
	int i_Temp;			// Holds the temporary Integral value
	int d_Temp;			// Holds the temporary Differential value
	int PWM_Duty;   // Holds PWM motor
	int PWM_Temp;   // Holds temporary PWM motor
	string Dir; // Holds Motor Direction 
};


//=============Overo Data Acquistion Function==================

string Append(int _1, int _2)
{
	stringstream converter;
	
	converter << _1 << _2;
	
	return converter.str();
}

long BinaryToDecimal(string bits)
{
	if(bits != "")
	{
		int decimal = 0;
		for(unsigned int i = (bits.length() - 1), j = 0; i >=0 && j<bits.length(); i--, j++)
		{
			switch(bits.at(j))
			{
				case '1':
				decimal += pow(2, i);
				break;
				case '0':
				break;
				default:
				decimal = -1;
				break;
			}
			if(decimal == -1)break;
		}
		return decimal;
	}
	return -1;
}


pid pidMotor(int ErrValue, pid control)
{
	const int iMax = 255;			// Used to prevent integral wind-up
	const int iMin = -255;		// Used to prevent integral wind-up
	pid mtrSign;
	mtrSign = control;
	
	// Calculates the Integral value
	mtrSign.P_Term = control.Kp * ErrValue;
	mtrSign.i_Temp += ErrValue;
	
	// Limits i_Temp from getting too positive or negative
	if (mtrSign.i_Temp > iMax)
	{mtrSign.i_Temp = iMax;}
	else if (mtrSign.i_Temp < iMin)
	{mtrSign.i_Temp = iMin;}
	
	// Calculates the Integral value
	mtrSign.I_Term = control.Ki * mtrSign.i_Temp;
	
	// Calculates Differential value
	mtrSign.D_Term = control.Kd * (mtrSign.d_Temp - ErrValue);
	mtrSign.d_Temp = ErrValue;
	
	/****** Now we have the P_Term, I_Term and D_Term *****/
	mtrSign.PWM_Duty = mtrSign.PWM_Temp - (mtrSign.P_Term + mtrSign.I_Term + mtrSign.D_Term);
	// PWM overflow prevention
	if (mtrSign.PWM_Duty > 255)
	{mtrSign.PWM_Duty = 255;}
	else if (mtrSign.PWM_Duty < -255)
	{mtrSign.PWM_Duty = -255;}
	
	if (mtrSign.PWM_Duty < 0)
	{mtrSign.Dir = LEFT;}
  else
	{mtrSign.Dir = RIGHT;}
	
  mtrSign.PWM_Temp = mtrSign.PWM_Duty;
	
	return mtrSign;
}

//=============END Overo Data Acquistion Function==============


//=============Object Tracking Function==================

void on_trackbar( int, void* )
{//This function gets called whenever a
	// trackbar position is changed
	
}

string intToString(int number){
	
	
	stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars()
{
	//create window for trackbars
	int H_MIN = 0;
	int H_MAX = 256;
	int S_MIN = 0;
	int S_MAX = 256;
	int V_MIN = 0;
	int V_MAX = 256;
	const string trackbarWindowName = "Trackbars";
	
	namedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
  
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
	createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
	createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
	createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
	createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
	createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );
}

void drawObject(int x, int y,Mat &frame){
	
	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!
	
	//UPDATE:JUNE 18TH, 2013
	//added 'if' and 'else' statements to prevent
	//memory errors from writing off the screen (ie. (-25,-25) is not within the window!)
	
	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
	if(y-25>0)
	line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
	else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
	if(y+25<IM_HEIGHT)
	line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
	else line(frame,Point(x,y),Point(x,IM_HEIGHT),Scalar(0,255,0),2);
	if(x-25>0)
	line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
	else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
	if(x+25<IM_WIDTH)
	line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
	else line(frame,Point(x,y),Point(IM_WIDTH,y),Scalar(0,255,0),2);
	
	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);
	
}

//**********************************************
void morphOps(Mat &thresh){
	
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
	
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));
	
	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);
	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
}

//**********************************************

void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed){
	
  const int MAX_OBJECT_AREA = IM_HEIGHT*IM_WIDTH/1.5;
	const int MIN_OBJECT_AREA = 20*20;
	const int MAX_NUM_OBJECTS = 50;
	bool objectFound = false;
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {
				
				Moments moment = moments((Mat)contours[index]);
				double area = moment.m00;
				
				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
				}else objectFound = false;
				
				
			}
			//let user know you found an object
			if(objectFound ==true){
				putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
				putText(cameraFeed,format("%d, %d", x,y),Point(0,250),2,1,Scalar(0,255,0),2);	
				//draw object location on screen
			drawObject(x,y,cameraFeed);}
			
		}else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

//=============END Object Tracking Function=================



//=============Overo Testing Function==================

void testingMemory()
{
	// Local variable
	int choice;
	string input="";
	int spdPan, spdTilt;
	string dirPan,dirTilt;
	long signControl=0;
	long value;
	
  while (true) {
		
		// Choose testing scenarion
		while (true) {
			
			cout << "Testing Menu" << endl;
			cout << "============" << endl;
			cout << "1. sPan = 128, sTilt = 0,  dirPan = IDLE, dirTilt = IDLE" << endl;
			cout << "2. sPan =  0, sTilt = 64, dirPan = IDLE, dirTilt = IDLE" << endl;
			cout << "3. sPan =  0, sTilt = 0,  dirPan = LEFT, dirTilt = IDLE"  << endl;
			cout << "4. sPan =  0, sTilt = 0,  dirPan = IDLE, dirTilt = LEFT"  << endl;
			cout << "5. sPan =  32,sTilt = 16, dirPan = LEFT, dirTilt = LEFT"  << endl;
			cout << "Please enter your Choice: ";
			getline(cin, input);
			
			// This code converts from string to number safely.
			stringstream myStream(input);
			if (myStream >> choice)
			break;
			cout << "Invalid number, please try again" << endl;
		}
		cout << "\n=== Run Scenario no: " << choice << " ==="<<endl << endl;
		
		switch (choice) {
			case	1	: spdPan = 128; spdTilt =  0; dirPan = IDLE; dirTilt = IDLE;  break;
			case	2	: spdPan =  0; spdTilt = 64; dirPan = IDLE; dirTilt = IDLE;  break;
			case	3 : spdPan =  0; spdTilt =  0; dirPan = LEFT; dirTilt = IDLE;  break;
			case	4 : spdPan =  0; spdTilt =  0; dirPan = IDLE; dirTilt = LEFT;  break;
			case	5 : spdPan = 32; spdTilt = 16; dirPan = LEFT; dirTilt = LEFT;  break;
			default : spdPan = 63; spdTilt = 31; dirPan = RIGHT;dirTilt = RIGHT; break;
		}
		
		string spdPan2 = bitset<8>(abs(spdPan)).to_string(); 
		string spdTilt2 = bitset<8>(abs(spdTilt)).to_string(); 
		
		// Note : Sec1 : 16384,  Sec2 : 64, Sec3 & Sec4 : 0, Sec5 :8208, Def : 16159 
		string msb_buffer_str = spdPan2+spdTilt2;
		
		// Note : 26 :BothLeft, 24 :dirPan 18: dirTilt
		string lsb_buffer_str = "000000000001" + dirPan + dirTilt;  
		signControl = BinaryToDecimal(msb_buffer_str+lsb_buffer_str);
		
		cout<<"Binary and Decimal representation"<<endl;
		cout<<msb_buffer_str + lsb_buffer_str<<endl;
		cout <<signControl<<endl<<endl;
		
		// Print value to Terminal from idx0 to idx19
		for (int i = 0; i<20; i++){
			//value = Device.getValue(i);
			//cout << "Read value from idx "<<i<<", result: " << value << endl;
			cout << "Read value from idx "<<i<<", result: " << signControl << endl;
		}
		cout<<"\n=== Finish printing to terminal ==="<<endl<<endl;
		
		
	} // End of testing loop
	
}

//=============END Overo Testing Function==============

void writeData(pid* X, pid* Y)
{
	long signControl = 0;
	string spdPan2 = bitset<8>(abs(X->PWM_Duty)).to_string(); 
	string spdTilt2 = bitset<8>(abs(Y->PWM_Duty)).to_string(); 
	
	// Note : Sec1 : 16384,  Sec2 : 64, Sec3 & Sec4 : 0, Sec5 :8208, Def : 16159 
	string msb_buffer_str = spdPan2+spdTilt2;
	
	// Note : 26 :BothLeft, 24 :dirPan 18: dirTilt
	string lsb_buffer_str = "000000000001" + X->Dir + Y->Dir;  
	signControl = BinaryToDecimal(msb_buffer_str+lsb_buffer_str);
	
	//cout<<"Binary and Decimal representation"<<endl;
	cout<<msb_buffer_str + lsb_buffer_str<<endl;
	cout <<signControl<<endl<<endl;
	
	//Write Value to FPGA
	//std::cout << "Set value to idx FPGA" << std::endl;
	//Device.setValue(signControl, 2);
	
	cout<<"\n=== Finish printing to terminal ==="<<endl<<endl;
	
	
}




//=============Camera Object Tracker Function=============
void pencariObject(bool pidfunc, pid* motorPan_PID, pid* motorTilt_PID)
{
	
	/* Data Acquisition Variable*/
	int setX = IM_WIDTH/2;
	int setY = IM_HEIGHT/2;
	
	int errValx = 0, errValy = 0;
	int THRESHOLD_CNT = 50;
	int posCounter = 0;
	
	//initial min and max HSV filter values.
	int H_MIN = 0;
	int H_MAX = 256;
	int S_MIN = 0;
	int S_MAX = 256;
	int V_MIN = 0;
	int V_MAX = 256;
	
	
	//names that will appear at the top of each window
	const string windowName = "Original Image";
	const string windowName1 = "HSV Image";
	const string windowName2 = "Thresholded Image";
	const string windowName3 = "After Morphological Operations";
	
	//some boolean variables for different functionality within this
	//program
	bool trackObjects = true;
	bool useMorphOps = true;
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	//matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold;
	//x and y values for the location of the object
	int x=0, y=0;
	int old_x=0, old_y=0;
	//create slider bars for HSV filtering
	//createTrackbars();
	const string trackbarWindowName = "Trackbars";
	
	namedWindow(trackbarWindowName,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
  
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
	createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
	createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
	createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
	createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
	createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open(0);
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH,IM_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,IM_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(1){
		//store image to matrix
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
		//filter HSV image between values and store filtered image to
		inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
		//perform morphological operations on thresholded image to eliminate noise
		//and emphasize the filtered object(s)
		if(useMorphOps)
		morphOps(threshold);
		//pass in thresholded frame to our object tracking function
		//this function will return the x and y coordinates of the
		//filtered object
		if(trackObjects)
		trackFilteredObject(x,y,threshold,cameraFeed);
		
		
		//pid function to control motor
		if(pidfunc)
		{
			
			if (posCounter <THRESHOLD_CNT)
			{
				errValx = x-setX;
				errValy = y-setY;
				std::cout << "Object Tracking Menu " <<x <<" " <<y<< std::endl; 
				*motorPan_PID   = pidMotor (errValx, *motorPan_PID);
				*motorTilt_PID = pidMotor (errValy, *motorTilt_PID);
				std::cout << "Error: " <<errValx<<" "<<errValy<<std::endl;
				
				if ((abs(errValx) < 20) && (abs(errValy)<20))
				{
					motorPan_PID->PWM_Duty = 0;
					motorTilt_PID->PWM_Duty = 0;
				}
				
				writeData(motorPan_PID,motorTilt_PID);
				
				std::cout << "PWM result: " <<motorPan_PID->PWM_Duty;
				std::cout << " " <<motorTilt_PID->PWM_Duty<< std::endl; 
				
				if (old_x == x && old_y == y)
				posCounter++;
			}
			
			// Detect no object on the screen
			if (posCounter == THRESHOLD_CNT)
			{
				std::cout << "No Object Detected"<<std::endl;
			}
			
			// Reset counter
			if (posCounter == THRESHOLD_CNT && (old_x != x || old_y != y))
			posCounter = 0;
			
			// Store old value of coordinate
			old_x = x;
			old_y = y;
		}
		
		//show frames 
		imshow(windowName2,threshold);
		imshow(windowName,cameraFeed);
		imshow(windowName1,HSV);
		
		
		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}
}

//=============Camera Object TrackerFunction==============








