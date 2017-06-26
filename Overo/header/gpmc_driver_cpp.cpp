/**
 * @file gpmc_driver_cpp.cpp
 * @brief Implementation file.
 * @author Jan Jaap Kempenaar, University of Twente
 */

#include "gpmc_driver_cpp.h"

// GPMC kernel module with definitions.
#include "rt_gpmc_user/rt_gpmc_fpga.h"

#include <fcntl.h>      // open()
#include <sys/ioctl.h>  // ioctl()
#include <unistd.h>     // close()

gpmc_driver::gpmc_driver(char* device)
{
  m_fd = open(device, 0);
  if (0 > m_fd)
  {
    m_fd = INVALID_DEVICE;
  }
}

gpmc_driver::~gpmc_driver()
{
  close(m_fd);
  m_fd = INVALID_DEVICE;
}

void gpmc_driver::setValue(unsigned int value, int idx)
{
  // create data structure and fill with data.
  struct gpmc_fpga_data temp;
  temp.data = value;
  temp.offset = idx;
  // Set value.
  ioctl(m_fd, IOCTL_SET_U32, &temp);
}

unsigned long gpmc_driver::getValue(int idx)
{
  // Read from specified address.
  struct gpmc_fpga_data temp;
  temp.offset = idx;
  ioctl(m_fd, IOCTL_GET_U32, &temp);
  return temp.data;
}

bool gpmc_driver::isValid()
{
  return (INVALID_DEVICE != m_fd);
}
