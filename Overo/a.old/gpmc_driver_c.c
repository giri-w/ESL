/**
 * @file gpmc_driver_c.c
 * @brief Implementation file.
 * @author Jan Jaap Kempenaar, University of Twente.
 */

#include "gpmc_driver_c.h"

// GPMC kernel module with definitions.
#include "rt_gpmc_user/rt_gpmc_fpga.h"

#include <fcntl.h>      // open()
#include <sys/ioctl.h>  // ioctl()
#include <unistd.h>     // close()

unsigned long getGPMCValue(int fd, int idx)
{
  // Read from specified address.
  struct gpmc_fpga_data temp;
  temp.offset = idx;
  ioctl(fd, IOCTL_GET_U32, &temp);
  return temp.data;
}


void setGPMCValue(int fd, unsigned int value, int idx)
{
  // create data structure and fill with data.
  struct gpmc_fpga_data temp;
  temp.data = value;
  temp.offset = idx;
  // Set value.
  ioctl(fd, IOCTL_SET_U32, &temp);
}