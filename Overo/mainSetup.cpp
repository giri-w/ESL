/**
 * @file example.cpp
 * @brief cpp example file.
 * @uathor Jan Jaap Kempenaar, University of Twente.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <bitset> // For converting integer to binary number
#include <cmath>
#include <stdint.h> // For storing value from FPGA (16 bit data)
#include <unistd.h> // function sleep(second) and usleep(microsecond)
#include "gpmc_driver_cpp.h"

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

// VARIABLE DECLARATION
// ========================

// Camera Set Point
int setX = IM_WIDTH/2;
int setY = IM_HEIGHT/2;



// FUNCTION DECLARATION
// ========================

std::string Append(int _1, int _2);
long BinaryToDecimal(std::string bits);
void testingMemory();





// MAIN PROGRAM
// ========================

int main(int argc, char* argv[])
{
	
	// Local Variable
	 int myMtrSpdPan 	  	= 0;
   int myMtrSpdTilt 	  = 0;
	 std::string dirPan   = "";
	 std::string dirTilt  = "";
	
	
  if (2 != argc)
  {
    std::cout << "Usage: " << argv[0] << " <device_name>" << std::endl;
    return 1;
  }
  
  std::cout << "GPMC driver cpp-example" << std::endl;
  // Create GPMC device driver object
  std::cout << "Opening gpmc_fpga..." << std::endl;
  gpmc_driver Device(argv[1]);
  
  if (!Device.isValid())
  {
    std::cerr << "Error opening gpmc_fpga device: %s" << std::endl;
    return 1;
  }
  
    // Read a value from idx 0
  long value = Device.getValue(0);
  std::cout << "Read value from idx 0, result: " << value << std::endl;
  
  // Write value to idx 2
  std::cout << "Set value to idx 2." << std::endl;
  value = 200;
  Device.setValue(signControl, 2);
	
	
	
	  if( curX > MAX_X )
        curX = MAX_X;
    if( curY > MAX_Y )
        curY = MAX_Y;
    
    if( curX < MIN_X )
        curX = MIN_X;
    if( curY < MIN_Y )
        curY = MIN_Y;
  
  std::cout << "Exiting..." << std::endl;
  return 0;
}


std::string Append(int _1, int _2)
{
    std::stringstream converter;

    converter << _1 << _2;

    return converter.str();
}

long BinaryToDecimal(std::string bits)
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

void testingMemory()
{
	// Local variable
	int choice;
	std::string input="";
	int spdPan, spdTilt;
	std::string dirPan,dirTilt;
	long signControl=0;
	long value;
	
  while (true) {
		
		// Choose testing scenarion
		while (true) {
			
		 std::cout << "Testing Menu" << std::endl;
		 std::cout << "============" << std::endl;
		 std::cout << "1. sPan = 64, sTilt = 0,  dirPan = IDLE, dirTilt = IDLE" << std::endl;
		 std::cout << "2. sPan =  0, sTilt = 64, dirPan = IDLE, dirTilt = IDLE" << std::endl;
		 std::cout << "3. sPan =  0, sTilt = 0,  dirPan = LEFT, dirTilt = IDLE"  << std::endl;
		 std::cout << "4. sPan =  0, sTilt = 0,  dirPan = IDLE, dirTilt = LEFT"  << std::endl;
		 std::cout << "5. sPan =  32,sTilt = 16, dirPan = LEFT, dirTilt = LEFT"  << std::endl;
		 std::cout << "Please enter your Choice: ";
		 std::getline(std::cin, input);

		 // This code converts from string to number safely.
		 std::stringstream myStream(input);
		 if (myStream >> choice)
		   break;
		 std::cout << "Invalid number, please try again" << std::endl;
 	 }
   std::cout << "=== Run Scenario no: " << choice << " ==="<<std::endl << std::endl;
		
  switch (choice) {
		case	1	: spdPan = 64; spdTilt =  0; dirPan = IDLE; dirTilt = IDLE;  break;
		case	2	: spdPan =  0; spdTilt = 64; dirPan = IDLE; dirTilt = IDLE;  break;
		case	3 : spdPan =  0; spdTilt =  0; dirPan = LEFT; dirTilt = IDLE;  break;
		case	4 : spdPan =  0; spdTilt =  0; dirPan = IDLE; dirTilt = LEFT;  break;
		case	5 : spdPan = 32; spdTilt = 16; dirPan = LEFT; dirTilt = LEFT;  break;
		default : spdPan = 63; spdTilt = 31; dirPan = RIGHT;dirTilt = RIGHT; break;
	}
	
	 std::string spdPan2 = std::bitset<8>(spdPan).to_string(); 
	 std::string spdTilt2 = std::bitset<8>(spdTilt).to_string(); 
	 
	 // Note : Sec1 : 16384,  Sec2 : 64, Sec3 & Sec4 : 0, Sec5 :8208, Def : 16159 
	 std::string msb_buffer_str = spdPan2+spdTilt2;
	 
	 // Note : 26 :BothLeft, 24 :dirPan 18: dirTilt
	 std::string lsb_buffer_str = "000000000001" + dirPan + dirTilt;  
	 signControl = BinaryToDecimal(msb_buffer_str+lsb_buffer_str);
	 
	 std::cout<<"Binary and Decimal representation"<<std::endl;
	 std::cout<<msb_buffer_str + lsb_buffer_str<<std::endl;
	 std::cout <<signControl<<std::endl;
 
 // Print value to Terminal from idx0 to idx19
   for (int i = 0; i<20; i++){
		value = Device.getValue(i);
		std::cout << "Read value from idx "<<i<<", result: " << value << std::endl;
	}
 
 
 } // End of testing loop

}


