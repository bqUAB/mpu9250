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

void setup();
void loop();

int main(){
  I2cBus i2c_2(2);  // Beagle Bone Black
  Mpu9250 imu(&i2c_2);

  printf("===== MPU 9250 Demo using Linux =====\n");
  /* Initiating communication */
  uint8_t c = imu.ComTest(kWhoAmImpu6500);
  if (c == 0x71){  // WHO_AM_I should always be 0x71
    printf("MPU9250 is online...\n");
  }

  // setup();
  // loop();

  return 0;
}

// void setup() {  // Arduino setup like
//   /* Initiating communication */
//   uint8_t c = imu.comTest(kWhoAmImpu6500);
//
//   if (c == 0x71){  // WHO_AM_I should always be 0x71
//     printf("MPU9250 is online...\n");
//
//     imu.initMPU9250();
//     imu.getGres();
//     imu.getAres();
//
//     /* Read the WIA register of the magnetometer, this is a good test of
//      * communication */
//     uint8_t d = imu.comTest(kWia);
//     if (d == 0x48){  // WHO_AM_I should always be 0x48
//       printf("AK8963 is online...\n");
//       imu.getMres();
//     } else {
//       perror("Could not connect to AK8963");
//       exit(1);
//     }
//   } else {
//     perror("Could not connect to MPU9250");
//     exit(1);
//   }
// }
//
// void loop() { while(1){  // Arduino loop like
//   /* If intPin goes high, all data registers have new data
//    * On interrupt, check if data ready interrupt */
//   if (imu.readByte(kIntStatus) & 0x01){
//
//     imu.readAccelData(imu.accelCount);  // Read the x/y/z adc values
//     /* Now we'll calculate the acceleration value into actual g's
//      * This depends on scale being set */
//     imu.ax = (float)imu.accelCount[0]*imu.aRes;
//     imu.ay = (float)imu.accelCount[1]*imu.aRes;
//     imu.az = (float)imu.accelCount[2]*imu.aRes;
//
//     imu.readGyroData(imu.gyroCount);  // Read the x/y/z adc values
//     /* Calculate the gyro value into actual degrees per second
//      * This depends on scale being set */
//     imu.gx = (float)imu.gyroCount[0]*imu.gRes;
//     imu.gy = (float)imu.gyroCount[1]*imu.gRes;
//     imu.gz = (float)imu.gyroCount[2]*imu.gRes;
//
//     imu.readMagData(imu.magCount);  // Read the x/y/z adc values
//     /* Get actual magnetometer value, this depends on scale being set */
//     imu.mx = (float)imu.magCount[0]*imu.mRes;
//     imu.my = (float)imu.magCount[1]*imu.mRes;
//     imu.mz = (float)imu.magCount[2]*imu.mRes;
//
//     imu.tempCount = imu.readTempData();  // Read the adc values
//   }
//
//   /* Print acceleration values in milligs! */
//   printf("X-acceleration: % 0.2f mg\n", 1000*imu.ax);
//   printf("Y-acceleration: % 0.2f mg\n", 1000*imu.ay);
//   printf("Z-acceleration: % 0.2f mg\n", 1000*imu.az);
//
//   /* Print gyro values in degree/sec */
//   printf("X-gyro rate: % 0.2f degrees/sec\n", imu.gx);
//   printf("Y-gyro rate: % 0.2f degrees/sec\n", imu.gy);
//   printf("Z-gyro rate: % 0.2f degrees/sec\n", imu.gz);
//
//   /* Print mag values in degree/sec */
//   printf("X-mag field: % 0.2f mG\n", imu.mx);
//   printf("Y-mag field: % 0.2f mG\n", imu.my);
//   printf("Z-mag field: % 0.2f mG\n", imu.mz);
//
//   /* Temperature in degrees Centigrade */
//   /* TEMP_degC = ((TEMP_OUT - RoomTemp_Offset)/Temp_Sensitivity) + 21degC
//    * Data found on MPU-9250 Product Specification section 3.4.2
//    * Sensitivity = 333.87 LSB/°C
//    * Room Temp Offset = 0 LSB */
//   imu.temperature = ((float) imu.tempCount) / 333.87 + 21.0;
//   // Print temperature in degrees Centigrade
//   printf("Temperature is % 0.2f degrees C\n", imu.temperature);
//
//   usleep(0.2*1000000);
// }}
