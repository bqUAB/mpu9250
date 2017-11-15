/*
 Note: The MPU9250 is an I2C sensor and uses the I2C Linux driver.
 */
#ifndef _MPU9250_H_
#define _MPU9250_H_

#include <stdint.h>  // Needed for unit uint8_t data type

// See also MPU-9250 Register Map and Descriptions, Revision 6.0,
// RM-MPU-9250A-00, Rev. 1.6, 01/07/2015 for registers not listed in above
// document.

#define WHO_AM_I_MPU9250  0x75 // Should return 0x71

uint8_t readByte(uint8_t address, uint8_t subAddress);

#endif // _MPU9250_H_
