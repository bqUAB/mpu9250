/*******************************************************************************
* Demo application using the MPU 9250
* by Byron Quezada
*
* Example of how to use the MPU 9250 through I2C using Linux by using an ARM
* processor running Debian. This demonstration shows how to write and read the
* MPU 9250 registers by accessing them through the built-in I2C kernel driver.
* The I2C adapter is selected from userspace through the /dev interface. The
* i2c-dev module is needed but it's loaded by default.
*
* Created 24 November 2017 based on:
* <http://elinux.org/Interfacing_with_I2C_Devices>
* <https://www.kernel.org/doc/Documentation/i2c/dev-interface>
* <https://github.com/sparkfun/MPU-9250_Breakout/blob/master/Libraries/Arduino/examples/MPU9250BasicAHRS/MPU9250BasicAHRS.ino>
*******************************************************************************/

#include <stdio.h>          // Needed for printf, snprintf, perror
#include <stdint.h>         // Needed for unit uint8_t data type
#include <stdlib.h>         // Needed for exit()
#include "i2c.h"
#include "mpu9250.h"


int main(){
  I2cBus i2c_bus(1);
  Mpu9250 imu(&i2c_bus);

  printf("===== MPU 9250 Demo using Linux =====\n");
  // Initiating communication
  uint8_t c = imu.ComTest(kWhoAmImpu6500);

  // Setup --------------------------------------------------------------------
  if (c == 0x71){  // WHO_AM_I should always be 0x71
    printf("MPU9250 is online...\n");

    imu.InitMpu9250();
    imu.GetGyroRes();
    imu.GetAccelRes();
    imu.GetMagnetomRes();

    // Read the WIA register of the magnetometer, this is a good test of
    // communication
    uint8_t d = imu.ComTest(kWia);
    if (d == 0x48){  // WHO_AM_I should always be 0x48
      printf("AK8963 is online...\n");
    } else {
      perror("Could not connect to AK8963");
      exit(1);
    }
  } else {
    perror("Could not connect to MPU9250");
    exit(1);
  }

  // End Setup ----------------------------------------------------------------

  while(1){  // Arduino loop like
    // If intPin goes high, all data registers have new data
    // On interrupt, check if data ready interrupt
    uint8_t data;
    i2c_bus.ReadFromMem(kMpu6500Addr, kIntStatus, &data);
    if ( data == 0x01){

      imu.ReadAccelData(imu.accel_count);  // Read the x/y/z adc values
      // /* Now we'll calculate the acceleration value into actual g's
      //  * This depends on scale being set
      imu.accel_x = (float)imu.accel_count[0]*imu.accel_res;
      imu.accel_y = (float)imu.accel_count[1]*imu.accel_res;
      imu.accel_z = (float)imu.accel_count[2]*imu.accel_res;

      imu.ReadGyroData(imu.gyro_count);  // Read the x/y/z adc values
      // /* Calculate the gyro value into actual degrees per second
      //  * This depends on scale being set
      imu.gyro_x = (float)imu.gyro_count[0]*imu.gyro_res;
      imu.gyro_y = (float)imu.gyro_count[1]*imu.gyro_res;
      imu.gyro_z = (float)imu.gyro_count[2]*imu.gyro_res;

      imu.ReadMagnetomData(imu.magnetom_count);  // Read the x/y/z adc values
      // /* Get actual magnetometer value, this depends on scale being set
      imu.magnetom_x = (float)imu.magnetom_count[0]*imu.magnetom_res;
      imu.magnetom_y = (float)imu.magnetom_count[1]*imu.magnetom_res;
      imu.magnetom_z = (float)imu.magnetom_count[2]*imu.magnetom_res;

      imu.temp_count = imu.ReadTempData();  // Read the adc values
    }

    // /* Print acceleration values in milligs!
    printf("X-acceleration: % 0.2f mg\n", 1000*imu.accel_x);
    printf("Y-acceleration: % 0.2f mg\n", 1000*imu.accel_y);
    printf("Z-acceleration: % 0.2f mg\n", 1000*imu.accel_z);

    // /* Print gyro values in degree/sec
    printf("X-gyro rate: % 0.2f degrees/sec\n", imu.gyro_x);
    printf("Y-gyro rate: % 0.2f degrees/sec\n", imu.gyro_y);
    printf("Z-gyro rate: % 0.2f degrees/sec\n", imu.gyro_z);

    // /* Print mag values in degree/sec
    printf("X-mag field: % 0.2f mG\n", imu.magnetom_x);
    printf("Y-mag field: % 0.2f mG\n", imu.magnetom_y);
    printf("Z-mag field: % 0.2f mG\n", imu.magnetom_z);

    // /* Temperature in degrees Centigrade
    // /* TEMP_degC = ((TEMP_OUT - RoomTemp_Offset)/Temp_Sensitivity) + 21degC
    //  * Data found on MPU-9250 Product Specification section 3.4.2
    //  * Sensitivity = 333.87 LSB/°C
    //  * Room Temp Offset = 0 LSB
    imu.temperature = ((float) imu.temp_count) / 333.87 + 21.0;
    // Print temperature in degrees Centigrade
    printf("Temperature is % 0.2f degrees C\n", imu.temperature);

    usleep(0.2*1000000);
  }
// */
  return 0;
}
