/**
 * @file example.cpp
 * @brief cpp example file.
 * @uathor Jan Jaap Kempenaar, University of Twente.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
#include <cmath>
#include "gpmc_driver_cpp.h"


std::string Append(int _1, int _2){
    std::stringstream converter;

    converter << _1 << _2;

    return converter.str();
}

long BinaryToDecimal(std::string bits)
{
    if(bits != "")
    {
        int decimal = 0;
        for(int i = (bits.length() - 1), j = 0; i >=0 && j<bits.length(); i--, j++)
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



int main(int argc, char* argv[])
{
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
  
  int counter = 0;
  int counterLimit = 2000;
  
  while (true) {
  
  
   if (counter == 0) {
  
   int myMtrSpd1 = 0;
   int myMtrSpd2 = 0;
   
   // Set duration of testing
   while (true) {
		 std::cout << "Please enter testing duration (def: 2000): ";
		 std::getline(std::cin, input);

		 // This code converts from string to number safely.
		 std::stringstream myStream(input);
		 if (myStream >> counterLimit)
		   break;
		 std::cout << "Invalid number, please try again" << std::endl;
 	 }
   std::cout << "Testing Duration: " << counterLimit << std::endl << std::endl;
   

 	 while (true) {
		 std::cout << "Please enter motor Speed1: ";
		 std::getline(std::cin, input);

		 // This code converts from string to number safely.
		 std::stringstream myStream(input);
		 if (myStream >> myMtrSpd1)
		   break;
		 std::cout << "Invalid number, please try again" << std::endl;
 	 }
   std::cout << "Your Speed1: " << myMtrSpd1 << std::endl << std::endl;
   
   while (true) {
		 std::cout << "Please enter motor Speed2: ";
		 std::getline(std::cin, input);

		 // This code converts from string to number safely.
		 std::stringstream myStream(input);
		 if (myStream >> myMtrSpd2)
		   break;
		 std::cout << "Invalid number, please try again" << std::endl;
 	 }
   std::cout << "Your Speed2: " << myMtrSpd2 << std::endl << std::endl;
   
		std::string dir1 = "";
		std::string dir2 = "";

 // How to get a string/sentence with spaces
 std::cout << "Please enter direction1 (10 : left, 01 : right):\n>";
 std::getline(std::cin, dir1);
 std::cout << "Your Direction1: " << dir1 << std::endl << std::endl;
 // How to get a string/sentence with spaces
 std::cout << "Please enter direction2 (10 : left, 01 : right):\n>";
 std::getline(std::cin, dir2);
 std::cout << "Your Direction2: " << dir2 << std::endl << std::endl;
 
 
 
 std::string spd1 = std::bitset<8>(myMtrSpd1).to_string(); 
 std::string spd2 = std::bitset<8>(myMtrSpd2).to_string(); 
 std::string msb_buffer_str = spd1+spd2;
 std::string lsb_buffer_str = "000000000000" + dir1 + dir2;
 std::cout<<msb_buffer_str + lsb_buffer_str<<std::endl;
 long signControl = BinaryToDecimal(msb_buffer_str+lsb_buffer_str);
 std::cout <<signControl<<std::endl;
 
 } // End of if statement
 
    
  
  // Read a value from idx 0
  long value = Device.getValue(0);
  std::cout << "Read value from idx 0, result: " << value << std::endl;
  
  // Write value to idx 2
  std::cout << "Set value to idx 2." << std::endl;
  value = 200;
  Device.setValue(signControl, 2);
  counter = counter + 1;
  
  if (counter < counterLimit) {
  	counter = 0;
  }
  
  
  
  }
  
  std::cout << "Exiting..." << std::endl;
  return 0;
}
