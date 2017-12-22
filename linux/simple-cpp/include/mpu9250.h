/*
 * Note: The MPU9250 is an I2C sensor and uses the I2C Linux driver.
 */
#ifndef _MPU9250_H_
#define _MPU9250_H_

#include <stdint.h>         // Needed for unit uint8_t data type
#include <stdio.h>          // Needed for printf, snprintf, perror
#include <stdlib.h>         // Needed for exit()
#include <sys/ioctl.h>      // Needed for ioctl
#include <linux/i2c-dev.h>  // Needed to use the I2C Linux driver (I2C_SLAVE)
#include <unistd.h>         // Needed for write, usleep
#include <math.h>           // Needed for pow

/* See also MPU-9250 Register Map and Descriptions, Revision 6.0,
 * RM-MPU-9250A-00, Rev. 1.6, 01/07/2015 for registers not listed in above
 * document. */

 WHO_AM_I_MPU9250
 WHO_AM_I_AK8963
 INT_STATUS

/* Using the MPU-9250 breakout board, ADO is set to 0
 * Seven-bit device address is 110100 for ADO = 0 and 110101 for ADO = 1 */
#define ADO 0
#if ADO
#define MPU9250_ADDRESS 0x69  // Device address when ADO = 1
#else
#define MPU9250_ADDRESS 0x68  // Device address when ADO = 0
#define AK8963_ADDRESS  0x0C   // Address of magnetometer
#endif // AD0

class MPU9250
{
  protected:
    /* Set initial input parameters */
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

    enum Mscale {
      MFS_14BITS = 0, // 0.6 mG per LSB
      MFS_16BITS      // 0.15 mG per LSB
    };

    /* Specify sensor full scale */
    uint8_t Gscale = GFS_250DPS;
    uint8_t Ascale = AFS_2G;
    /* Choose either 14-bit or 16-bit magnetometer resolution */
    uint8_t Mscale = MFS_16BITS;
    /* 2 for 8 Hz, 6 for 100 Hz continuous magnetometer data read */
    uint8_t Mmode = 0x02;

  public:
    int* ptrFile;

    float SelfTest[6];
    /* Bias corrections for gyro and accelerometer */
    float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0};
    /* Factory magnetometer calibration and magnetometer bias */
    float magCalibration[3] = {0, 0, 0}, magbias[3] = {0, 0, 0};
    /* Stores the 16-bit signed sensor output */
    int16_t accelCount[3];  // Accelerometer
    int16_t gyroCount[3];   // Gyroscope
    int16_t magCount[3];    // Magnetometer
    // Scale resolutions per LSB for the sensors
    float aRes, gRes, mRes;
    // Variables to hold latest sensor data values
    float ax, ay, az, gx, gy, gz, mx, my, mz;

    int16_t tempCount;  // Temperature raw count output
    float temperature;  // Stores the real internal chip temperature in Celsius

  private:
  void chooseDevice(uint8_t devAdd);

  public:
    void writeByte(uint8_t regAdd, uint8_t data);
    uint8_t readByte(uint8_t regAdd);
    void readBytes(uint8_t regAdd, uint8_t count, uint8_t* data);
    uint8_t comTest(uint8_t WHO_AM_I);
    void readAccelData(int16_t* destination);
    void readGyroData(int16_t* destination);
    void readMagData(int16_t* destination);
    void getAres();
    void getGres();
    void getMres();
    int16_t readTempData();
};  // class MPU9250

#endif // _MPU9250_H_
