/**
 * @file gpmc_driver_cpp.h
 * @brief CPP driver to use with the GPMC kernel module.
 * @author Jan Jaap Kempenaar, University of Twente
 */

#pragma once

#define INVALID_DEVICE  -1

class gpmc_driver
{
public:
  /**
    * @brief Class constructor.
    * 
    * Open connection with the FPGA hardware device.
    * @param device Device identifier string.
    */
  gpmc_driver(char* device);
  
  /**
    * @brief Class destructor.
    * 
    * Close connection with the FPGA hardware device.
    */
  ~gpmc_driver();
  
  /**
    * @brief Set value for the specified hardware component.
    * 
    * Set the value for the specific hardware registers.
    * Note: Linux writes 32-bit value, while the registers in the GPMC driver on the FPGA side are 16-bits.
    * @param value 32-bit value that is written to the FPGA.
    * @param idx Register offset, see the VHDL file which registers are affected.
    */
  void setValue(unsigned int value, int idx);
  
  /**
    * @brief Get value for the specified hardware component.
    * 
    * Get the value for the specified hardware component on the FPGA.
    * Although 32-bit integers are read, the ioctl function returns unsigned long values.
    * 
    * @param idx Register offset, see the VHDL file which registers are affected.
    * @return Returns the value read on the component's address.
    */
  unsigned long getValue(int idx);
  
  /**
    * @brief Check validity of the file descriptor
    */
  bool isValid();
private:
  int m_fd;   ///< File descriptor for connection with the GPMC FPGA device.
};
