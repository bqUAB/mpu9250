/*
 Note: The MPU9250 is an I2C sensor and uses the I2C Linux driver.
 */
#ifndef _MPU9250_H_
#define _MPU9250_H_

#include <stdint.h>   // Needed for unit uint8_t data type
#include <stdbool.h>  // Needed for bool type

// See also MPU-9250 Register Map and Descriptions, Revision 6.0,
// RM-MPU-9250A-00, Rev. 1.6, 01/07/2015 for registers not listed in above
// document.

#define SMPLRT_DIV        0x19
#define CONFIG            0x1A
#define GYRO_CONFIG       0x1B
#define ACCEL_CONFIG      0x1C
#define ACCEL_CONFIG2     0x1D

#define ACCEL_XOUT_H      0x3B
#define GYRO_XOUT_H       0x43
#define WHO_AM_I_MPU9250  0x75 // Should return 0x71

// Using the Sparkfun MPU-9250 breakout board, ADO is set to 0
// Seven-bit device address is 110100 for ADO = 0 and 110101 for ADO = 1
#define ADO 0
#if ADO
#define MPU9250_ADDRESS 0x69  // Device address when ADO = 1
#else
#define MPU9250_ADDRESS 0x68  // Device address when ADO = 0
#define AK8963_ADDRESS  0x0C  // Address of magnetometer
#endif // AD0

void MPU9250SelfTest(int file, float * destination);

#endif // _MPU9250_H_
