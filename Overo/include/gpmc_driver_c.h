/**
 * @file gpmc_driver_c.h
 * @brief C driver to use with the GPMC kernel module.
 * @author Jan Jaap Kempenaar, University of Twente
 */

#ifndef _GPMC_DRIVER_C_H_
#define _GPMC_DRIVER_C_H_

/**
  * @brief Set value to the given registers of idx.
  * 
  * Set the value for the specific hardware registers.
  * Note: Linux writes 32-bit value, while the registers in the GPMC driver on the FPGA side are 16-bits.
  * 
  * @param fd File descriptor for GPMC device.
  * @param value 32-bit value that is written to the FPGA.
  * @param idx Register offset, see the VHDL file which registers are affected.
  */
void setGPMCValue(int fd, unsigned int value, int idx);

/**
  * @brief Get value from the specified registers in idx.
  * 
  * Get the value for the specified hardware component on the FPGA.
  * Although 32-bit integers are read, the ioctl function returns unsigned long values.
  * 
  * @param fd File descriptor for GPMC device.
  * @param idx Register offset, see the VHDL file which registers are affected.
  * @return Returns the value in the register.
  */
unsigned long getGPMCValue(int fd, int idx);

#endif // _GPMC_DRIVER_C_H_
