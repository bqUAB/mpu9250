/*
 Note: The MPU9250 is an I2C sensor and uses the I2C Linux driver.
 */
#ifndef _MPU9250_H_
#define _MPU9250_H_

#include <stdint.h>   // Needed for unit uint8_t data type
#include <stdbool.h>  // Needed for bool type
#include <time.h>     // Needed for nanosleep
#include <math.h>     // Needed for pow

// See also MPU-9250 Register Map and Descriptions, Revision 6.0,
// RM-MPU-9250A-00, Rev. 1.6, 01/07/2015 for registers not listed in above
// document.

#define SELF_TEST_X_GYRO   0x00
#define SELF_TEST_Y_GYRO   0x01
#define SELF_TEST_Z_GYRO   0x02
#define SELF_TEST_X_ACCEL  0x0D
#define SELF_TEST_Y_ACCEL  0x0E
#define SELF_TEST_Z_ACCEL  0x0F

#define SMPLRT_DIV     0x19
#define CONFIG         0x1A
#define GYRO_CONFIG    0x1B
#define ACCEL_CONFIG   0x1C
#define ACCEL_CONFIG2  0x1D

#define ACCEL_XOUT_H  0x3B

#define GYRO_XOUT_H  0x43

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

typedef struct _MPU9250 {  // struct is a data type so you cannot set initial
                          // input parameters. Remember to declare them.
  // Specify sensor full scale
  uint8_t Gscale, Ascale;

  // Stores the 16-bit signed sensor output
  int16_t gyroCount[3], accelCount[3];

  // Scale resolutions per LSB for the sensors
  float aRes, gRes;

  // Variables to hold latest sensor data values
  float ax, ay, az, gx, gy, gz;

  // Bias corrections for gyro and accelerometer
  float gyroBias[3], accelBias[3];
  float SelfTest[6];

} MPU9250;

bool MPU9250_STRUCT_INI(struct _MPU9250 * myIMU);
void MPU9250_SELF_TEST(int file, float * destination);

#endif // _MPU9250_H_
