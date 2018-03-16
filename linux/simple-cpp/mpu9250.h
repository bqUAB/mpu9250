// The MPU-9250 combines two chips: the MPU-6500, which contains a 3-axis
// gyroscope, a 3-axis accelerometer, and the AK8963 3-axis digital compass.

#ifndef MPU9250_H_
#define MPU9250_H_

#include <stdint.h>         // Needed for unit uint8_t data type
#include <sys/ioctl.h>      // Needed for ioctl
// #include <linux/i2c-dev.h>  // Needed to use the I2C Linux driver (I2C_SLAVE)
#include <stdio.h>          // Needed for printf, snprintf, perror
#include <stdlib.h>         // Needed for exit()
#include <unistd.h>         // Needed for write, usleep
#include "i2c.h"

/* See also MPU-9250 Register Map and Descriptions, Revision 6.0,
 * RM-MPU-9250A-00, Rev. 1.6, 01/07/2015 for registers not listed in above
 * document. */

// MPU6500 Registers
/* Using the MPU-9250 breakout board, ADO is set to 0
   Seven-bit device address is 110100 for ADO = 0 and 110101 for ADO = 1 */

// const uint8_t kMpu6500Addr = 0x69;  // Device address when ADO = 1
const uint8_t kMpu6500Addr = 0x68;  // Device address when ADO = 0

                                    // default value
const uint8_t kSmplrtDiv    = 0x19;  // 0x00
const uint8_t kConfig       = 0x1A; // 0x00
const uint8_t kGyroConfig   = 0x1B;  // 0x00
const uint8_t kAccelConfig  = 0x1C;  // 0x00
const uint8_t kAccelConfig2 = 0x1D;  // 0x00

const uint8_t kI2cMstCtrl = 0x24;  // 0x00

const uint8_t kIntPinCfg = 0x37;  // 0x00
const uint8_t kIntEnable = 0x38;  // 0x00

const uint8_t kIntStatus  = 0x3A;
const uint8_t kAccelXoutH = 0x3B;
const uint8_t kAccelXoutL = 0x3C;
const uint8_t kAccelYoutH = 0x3D;
const uint8_t kAccelYoutL = 0x3E;
const uint8_t kAccelZoutH = 0x3F;
const uint8_t kAccelZoutL = 0x40;
const uint8_t kTempOutH   = 0x41;
const uint8_t kTempOutL   = 0x42;
const uint8_t kGyroXoutH  = 0x43;
const uint8_t kGyroXoutL  = 0x44;
const uint8_t kGyroYoutH  = 0x45;
const uint8_t kGyroYoutL  = 0x46;
const uint8_t kGyroZoutH  = 0x47;
const uint8_t kGyroZoutL  = 0x48;

const uint8_t kWhoAmImpu6500 = 0x75;  // Should return 0x71

// Magnetometer AK8963 Registers
const uint8_t kAk8963Addr = 0x0C;

const uint8_t kWia  = 0x00;  // should return 0x48
const uint8_t kInfo = 0x01;
const uint8_t kSt1  = 0x02;  // data ready status bit 0
const uint8_t kHxl  = 0x03;  // data
const uint8_t kHxh  = 0x04;
const uint8_t kHyl  = 0x05;
const uint8_t kHyh  = 0x06;
const uint8_t kHzl  = 0x07;
const uint8_t kHzh  = 0x08;

class Mpu9250 {
  protected:
    I2cBus* ptr_i2c;
    /* Set initial input parameters */
    enum GyroScale {
      kGfs250Dps = 0,
      kGfs500Dps,
      kGfs1000Dps,
      kGfs2000Dps
    };

    enum AccelScale {
      kAfs2G = 0,
      kAfs4G,
      kAfs8G,
      kAfs16G
    };

    enum MagnetomScale {
      kMfs14Bits = 0, // 0.6 mG per LSB
      kMfs16Bits      // 0.15 mG per LSB
    };

    /* Specify sensor full scale */
    uint8_t gyro_scale = kGfs250Dps;
    uint8_t accel_scale = kAfs2G;
    /* Choose either 14-bit or 16-bit magnetometer resolution */
    uint8_t magnetom_scale = kMfs16Bits;
    /* 2 for 8 Hz, 6 for 100 Hz continuous magnetometer data read */
    uint8_t m_mode = 0x02;

  public:
    Mpu9250(I2cBus* i2c_n);

    /* Stores the 16-bit signed sensor output */
    int16_t accel_count[3];  // Accelerometer
    int16_t gyro_count[3];  // Gyroscope
    int16_t magnetom_count[3];  // Magnetometer
    // Scale resolutions per LSB for the sensors
    float accel_res, gyro_res, magnetom_res;
    // Variables to hold latest sensor data values
    float ax, ay, az, gx, gy, gz, mx, my, mz;

    int16_t temp_count;  // Temperature raw count output
    float temperature;  // Stores the real internal chip temperature in Celsius

  private:
  void ChooseDevice(bool magnetom);

  public:
    uint8_t ComTest(uint8_t test_who);
    void InitMpu9250();
    void ReadAccelData(int16_t* destination);
    void ReadGyroData(int16_t* destination);
    void ReadMagnetomData(int16_t* destination);
    void GetGyroRes();
    void GetAccelRes();
    void GetMagnetomRes();
    int16_t ReadTempData();
};  // class MPU9250

#endif // MPU9250_H_
