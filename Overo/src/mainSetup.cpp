/**
	* @file example.cpp
	* @brief cpp example file.
	* @uathor Jan Jaap Kempenaar, University of Twente.
*/
#include <iostream>
#include <string>
#include <sstream>
#include <opencv2/core.hpp> 						//opencv2 library
#include <opencv2/imgproc/imgproc.hpp>  //opencv2 library
#include <opencv2/highgui.hpp> 					//opencv2 library
#include "objectTrack_cpp.h"
#include "../include/gpmc_driver_cpp.h"


//using namespace cv;

// VARIABLE DECLARATION
// ========================

/* PID Controller*/
pid motorPan_PID,motorTilt_PID;


// MAIN PROGRAM
// ========================

int main(int argc, char* argv[])
{
	
	
	// Setting PID Gain for motorPan and motor Tilt
	motorPan_PID.Kp  = 0.01;	motorTilt_PID.Kp  = 0.01;	
	motorPan_PID.Ki  = 0.01;	motorTilt_PID.Ki  = 0.01;	
	motorPan_PID.Kd  = 0.01;  motorTilt_PID.Kd  = 0.01;
	
	// Other Variables
	std::string input="";
	int choice;
	
	//===================================
	
	// Initialize GPMC connection
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
	
	while (true){
		
		// Program Selection
		while (true) {
			
			std::cout << "Program Selection" << std::endl;
			std::cout << "=================" << std::endl;
			std::cout << "1. Testing Section" << std::endl;
			std::cout << "2. Object Tracking" << std::endl;
			std::cout << "3. Object Tracking + PID Controller"  << std::endl;
			std::cout << "4. PID Setting"  << std::endl;
			std::cout << "5. Exit"  << std::endl;
			std::cout << "Please enter your Choice: ";
			std::getline(std::cin, input);
			
			// This code converts from string to number safely.
			std::stringstream myStream(input);
			if (myStream >> choice)
			break;
			std::cout << "Invalid number, please try again" << std::endl;
		}
		std::cout << "\n=== Run Program no: " << choice << " ==="<<std::endl << std::endl;
		
		switch (choice) {
			case	1	: testingMemory();  break;
			case	2	: pencariObject(false,&motorPan_PID, &motorTilt_PID);  break;
			case	3 : pencariObject(true, &motorPan_PID, &motorTilt_PID); break;
			case	4 : std::cout << "PID Setting" << std::endl; break;
			case	5 : std::cout << "Sampai Jumpa" << std::endl;  break;
			default : std::cout << "Masukkan pilihan lainnya " << std::endl; break;
		}
	}
	
	std::cout << "Exiting..." << std::endl;
  return 0;
}


