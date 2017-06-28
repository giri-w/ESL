#ifndef _OBJECT_TRACK_
	#define _OBJECT_TRACK_
	
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


 //using namespace cv;
 //using namespace std;
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
		std::string Dir; // Holds Motor Direction 
	};
	
	


	// Overo Data Acquisition 
	std::string Append(int _1, int _2);
	long BinaryToDecimal(std::string bits);
	pid pidMotor(int ErrValue, pid control);
	
	// Object Tracking
	void on_trackbar( int, void*);
	std::string intToString(int number);
	void createTrackbars();
	void drawObject(int x, int y,cv::Mat &frame);
	void morphOps(cv::Mat &thresh);
	void trackFilteredObject(int &x, int &y, cv::Mat threshold, cv::Mat &cameraFeed); 

			

	// Object Tracker Function
  void testingMemory();
  void writeData(pid* X, pid* Y);
	void pencariObject(bool pidfunc, pid* motorPan_PID, pid* motorTilt_PID);
	
#endif
