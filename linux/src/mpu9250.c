#include <mpu9250.h>

// api for register access, defined in main.c
bool MPU9250_REG_WRITE(int file, uint8_t reg_add, uint8_t value);
bool MPU9250_REG_READ(int file, uint8_t reg_add, uint8_t *value);
bool MPU9250_REG_MULTI_READ(int file, uint8_t reg_add,\
                             uint8_t count, uint8_t *data);

// Time to sleep, similar to delay function
struct timespec req = {0};

bool MPU9250_STRUCT_INI(struct _MPU9250 * myIMU){
  bool bSuccess = false;

  enum Gscale {
    GFS_250DPS = 0,
    GFS_500DPS,
    GFS_1000DPS,
    GFS_2000DPS
  };

  enum Ascale {
      AFS_2G = 0,
      AFS_4G,
      AFS_8G,
      AFS_16G
    };

  // Specify sensor full scale
  myIMU->Gscale = GFS_250DPS;
  myIMU->Ascale = AFS_2G;

  // Bias corrections for gyro and accelerometer
  for(int i = 0; i < 3; i++){
    myIMU->gyroBias[i]  = 0;
    myIMU->accelBias[i] = 0;
  }

  return bSuccess;
}

//==============================================================================
//= Set of useful function to access acceleration. gyroscope, magnetometer,    =
//= and temperature data                                                       =
//==============================================================================

// Accelerometer and gyroscope self test; check calibration wrt factory settings
void MPU9250_SELF_TEST(int file, float * destination){
  // Should return percent deviation from factory trim values, +/- 14 or less
  // deviation is a pass
  uint8_t rawData[6] = {0, 0, 0, 0, 0, 0};
  uint8_t selfTest[6];
  int16_t gAvg[3], gSTAvg[3], aAvg[3], aSTAvg[3];
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

  // get average current values of gyro and accelerometer
  for(int i = 0; i < 200; i++) {
    // Read the six raw data registers sequentially into data array
    MPU9250_REG_MULTI_READ(file, GYRO_XOUT_H, 6, &rawData[0]);
    // Turn the MSB and LSB into a signed 16-bit value
    gAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;
    gAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
    gAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;

    // Read the six raw data registers into data array
    MPU9250_REG_MULTI_READ(file, ACCEL_XOUT_H, 6, &rawData[0]);
    // Turn the MSB and LSB into a signed 16-bit value
    aAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;
    aAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
    aAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
  }

  // Get average of 200 values and store as average current readings
  for(int i =0; i < 3; i++) {
    gAvg[i] /= 200;
    aAvg[i] /= 200;
  }

  /* ---------- Configure the accelerometer and gyro for self-test ---------- */
  // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
  MPU9250_REG_WRITE(file, GYRO_CONFIG, 0xE0);
  // Enable self test on all three axes and set accelerometer range to +/- 2 g
  MPU9250_REG_WRITE(file, ACCEL_CONFIG, 0xE0);
  // Sleep a while to let the device stabilize
  req.tv_sec = 0;  // sleeps no longer than 1 second
  req.tv_nsec = 25 * 1000000L;  // 25 milliseconds
  nanosleep(&req, (struct timespec *)NULL);

  // get average self-test values of gyro and acclerometer
  for(int i = 0; i < 200; i++) {
    // Read the six raw data registers sequentially into data array
    MPU9250_REG_MULTI_READ(file, GYRO_XOUT_H, 6, &rawData[0]);
    // Turn the MSB and LSB into a signed 16-bit value
    gSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;
    gSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
    gSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;

    // Read the six raw data registers into data array
    MPU9250_REG_MULTI_READ(file, ACCEL_XOUT_H, 6, &rawData[0]);
    // Turn the MSB and LSB into a signed 16-bit value
    aSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);
    aSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);
    aSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]);
  }

  // Get average of 200 values and store as average self-test readings
  for(int i =0; i < 3; i++) {
    gSTAvg[i] /= 200;
    aSTAvg[i] /= 200;
  }

  // Configure the gyro and accelerometer for normal operation
  MPU9250_REG_WRITE(file, GYRO_CONFIG, 0x00);
  MPU9250_REG_WRITE(file, ACCEL_CONFIG, 0x00);
  // Sleep a while to let the device stabilize
  req.tv_sec = 0;  // sleeps no longer than 1 second
  req.tv_nsec = 25 * 1000000L;  // 25 milliseconds
  nanosleep(&req, (struct timespec *)NULL);

  /* Retrieve accelerometer and gyro factory Self-Test Code from USR_Reg      */

  // X-axis accel self-test results
  MPU9250_REG_READ(file, SELF_TEST_X_ACCEL, selfTest);
  // Y-axis accel self-test results
  MPU9250_REG_READ(file, SELF_TEST_Y_ACCEL, selfTest+1);
  // Z-axis accel self-test results
  MPU9250_REG_READ(file, SELF_TEST_Z_ACCEL, selfTest+2);
  // X-axis gyro self-test results
  MPU9250_REG_READ(file, SELF_TEST_X_GYRO, selfTest+3);
  // Y-axis gyro self-test results
  MPU9250_REG_READ(file, SELF_TEST_Y_GYRO, selfTest+4);
  // Z-axis gyro self-test results
  MPU9250_REG_READ(file, SELF_TEST_Z_GYRO, selfTest+5);

  /* Retrieve factory self-test value from self-test code reads               */
  // FT[Xa] factory trim calculation
  factoryTrim[0] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[0] - 1.0)));
  // FT[Ya] factory trim calculation
  factoryTrim[1] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[1] - 1.0)));
  // FT[Za] factory trim calculation
  factoryTrim[2] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[2] - 1.0)));
  // FT[Xg] factory trim calculation
  factoryTrim[3] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[3] - 1.0)));
  // FT[Yg] factory trim calculation
  factoryTrim[4] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[4] - 1.0)));
  // FT[Zg] factory trim calculation
  factoryTrim[5] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[5] - 1.0)));

  /* Report results as a ratio of (STR - FT)/FT; the change from Factory Trim
     of the Self-Test Response                                                */
  // To get percent, must multiply by 100
  for (int i = 0; i < 3; i++) {
    // Report percent differences
    destination[i]   = 100.0*((float)(aSTAvg[i] - aAvg[i]))/factoryTrim[i];
    // Report percent differences
    destination[i+3] = 100.0*((float)(gSTAvg[i] - gAvg[i]))/factoryTrim[i+3];
  }
}
