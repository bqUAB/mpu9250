#include <mpu9250.h>

// api for register access, defined in main.c
bool MPU9250_REG_WRITE(int file, uint8_t address, uint8_t value);
bool MPU9250_REG_READ(int file, uint8_t address,uint8_t *value);

//==============================================================================
//= Set of useful function to access acceleration. gyroscope, magnetometer,    =
//= and temperature data                                                       =
//==============================================================================

// Accelerometer and gyroscope self test; check calibration wrt factory settings
void MPU9250SelfTest(int file, float * destination){
  // Should return percent deviation from factory trim values, +/- 14 or less
  // deviation is a pass
  uint8_t rawData[6] = {0, 0, 0, 0, 0, 0};
  uint8_t selfTest[6];
  int16_t gAvg[3], aAvg[3], aSTAvg[3], gSTAvg[3];
  float factoryTrim[6];
  uint8_t FS = 0;

  // Set gyro sample rate to 1 kHz
  MPU9250_REG_WRITE(file, SMPLRT_DIV, 0x00);
  // Set gyro sample rate to 1 kHz and DLPF to 92 Hz
  MPU9250_REG_WRITE(file, CONFIG, 0x02);
  // Set full scale range for the gyro to 250 dps
  MPU9250_REG_WRITE(file, GYRO_CONFIG, 1<<FS);
  // Set accelerometer rate to 1 kHz and bandwidth to 92 Hz
  MPU9250_REG_WRITE(file, ACCEL_CONFIG2, 0x02);
  // Set full scale range for the accelerometer to 2 g
  MPU9250_REG_WRITE(file, ACCEL_CONFIG, 1<<FS);
}

//
//   for( int ii = 0; ii < 200; ii++) {  // get average current values of gyro and acclerometer
//
//     readBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);        // Read the six raw data registers into data array
//     aAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
//     aAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
//     aAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
//
//     readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);       // Read the six raw data registers sequentially into data array
//     gAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
//     gAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
//     gAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
//   }
//
//   for (int ii =0; ii < 3; ii++) {  // Get average of 200 values and store as average current readings
//     aAvg[ii] /= 200;
//     gAvg[ii] /= 200;
//   }
//
// // Configure the accelerometer for self-test
//   writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0xE0); // Enable self test on all three axes and set accelerometer range to +/- 2 g
//   writeByte(MPU9250_ADDRESS, GYRO_CONFIG,  0xE0); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
//   delay(25);  // Delay a while to let the device stabilize
//
//   for( int ii = 0; ii < 200; ii++) {  // get average self-test values of gyro and acclerometer
//
//     readBytes(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers into data array
//     aSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
//     aSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
//     aSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
//
//     readBytes(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
//     gSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
//     gSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
//     gSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
//   }
//
//   for (int ii =0; ii < 3; ii++) {  // Get average of 200 values and store as average self-test readings
//     aSTAvg[ii] /= 200;
//     gSTAvg[ii] /= 200;
//   }
//
//   // Configure the gyro and accelerometer for normal operation
//   writeByte(MPU9250_ADDRESS, ACCEL_CONFIG, 0x00);
//   writeByte(MPU9250_ADDRESS, GYRO_CONFIG,  0x00);
//   delay(25);  // Delay a while to let the device stabilize
//
//   // Retrieve accelerometer and gyro factory Self-Test Code from USR_Reg
//   selfTest[0] = readByte(MPU9250_ADDRESS, SELF_TEST_X_ACCEL); // X-axis accel self-test results
//   selfTest[1] = readByte(MPU9250_ADDRESS, SELF_TEST_Y_ACCEL); // Y-axis accel self-test results
//   selfTest[2] = readByte(MPU9250_ADDRESS, SELF_TEST_Z_ACCEL); // Z-axis accel self-test results
//   selfTest[3] = readByte(MPU9250_ADDRESS, SELF_TEST_X_GYRO);  // X-axis gyro self-test results
//   selfTest[4] = readByte(MPU9250_ADDRESS, SELF_TEST_Y_GYRO);  // Y-axis gyro self-test results
//   selfTest[5] = readByte(MPU9250_ADDRESS, SELF_TEST_Z_GYRO);  // Z-axis gyro self-test results
//
//   // Retrieve factory self-test value from self-test code reads
//   factoryTrim[0] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[0] - 1.0) )); // FT[Xa] factory trim calculation
//   factoryTrim[1] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[1] - 1.0) )); // FT[Ya] factory trim calculation
//   factoryTrim[2] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[2] - 1.0) )); // FT[Za] factory trim calculation
//   factoryTrim[3] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[3] - 1.0) )); // FT[Xg] factory trim calculation
//   factoryTrim[4] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[4] - 1.0) )); // FT[Yg] factory trim calculation
//   factoryTrim[5] = (float)(2620/1<<FS)*(pow( 1.01 , ((float)selfTest[5] - 1.0) )); // FT[Zg] factory trim calculation
//
//  // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
//  // To get percent, must multiply by 100
//   for (int i = 0; i < 3; i++) {
//     destination[i]   = 100.0*((float)(aSTAvg[i] - aAvg[i]))/factoryTrim[i];   // Report percent differences
//     destination[i+3] = 100.0*((float)(gSTAvg[i] - gAvg[i]))/factoryTrim[i+3]; // Report percent differences
//   }
// }
