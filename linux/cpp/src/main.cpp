/*******************************************************************************
* Demo application using the MPU 9250
* by Byron Quezada
*
* Example of how to use the MPU 9250 throuh I2C using Linux by using an ARM
* processor running Debian. This demonstration shows how to write and read the
* MPU 9250 registers by accessing them through the built-in I2C kernel driver.
* The I2C adapter is selected from userspace through the /dev interface. The
* i2c-dev module is needed but it's loaded by default.
*
* Created 24 November 2017 based on:
* <http://elinux.org/Interfacing_with_I2C_Devices>
* <https://www.kernel.org/doc/Documentation/i2c/dev-interface>
*******************************************************************************/

#include <stdio.h>          // Needed for printf, snprintf, perror
#include <stdint.h>         // Needed for unit uint8_t data type
#include <mpu9250.h>

MPU9250 myIMU;

int main(){

  printf("===== MPU 9250 Demo using Linux =====\n");
  myIMU.open_i2c();
  // Initiating communication
  myIMU.choose_device(MPU9250_ADDRESS);
/*
  // Read the WHO_AM_I register, this is a good test of communication
  uint8_t c = myIMU.readByte(file, WHO_AM_I_MPU9250);
  printf("WHO_AM_I: %#X\t", c);
  printf("I should be: 0x71\n");

  if (c == 0x71){  // WHO_AM_I should always be 0x71
    printf("MPU9250 is online...\n");

    // Start by performing self test and reporting values
    // myIMU.MPU9250SelfTest(myIMU.SelfTest);
    // printf("Accelerometer Self Test\n");
    // printf("x-axis self test: acceleration trim within : ");
    // printf("% 0.2f%% of factory value\n", myIMU.SelfTest[0]);
    // printf("y-axis self test: acceleration trim within : ");
    // printf("% 0.2f%% of factory value\n", myIMU.SelfTest[1]);
    // printf("z-axis self test: acceleration trim within : ");
    // printf("% 0.2f%% of factory value\n", myIMU.SelfTest[2]);
    // printf("Gyroscope Self Test\n");
    // printf("x-axis self test: gyration trim within : ");
    // printf("% 0.2f%% of factory value\n", myIMU.SelfTest[3]);
    // printf("y-axis self test: gyration trim within : ");
    // printf("% 0.2f%% of factory value\n", myIMU.SelfTest[4]);
    // printf("z-axis self test: gyration trim within : ");
    // printf("% 0.2f%% of factory value\n", myIMU.SelfTest[5]);
  }
*/
  return 0;
}
