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
#include <stdlib.h>         // Needed for exit()
#include <fcntl.h>          // Needed for open()
#include <sys/ioctl.h>      // Needed for ioctl
#include <linux/i2c-dev.h>  // Needed to use the I2C Linux driver (I2C_SLAVE)
#include <mpu9250.h>

MPU9250 myIMU;

void choose_device(int file, uint8_t dev_add){
  // Specify device address to communicate
  if (ioctl(file, I2C_SLAVE, dev_add) < 0){
    printf("Failed to acquire bus access and/or talk to slave.\n");
    /* ERROR HANDLING; you can check errno to see what went wrong */
    exit(1);
  }
}

int main(){

  printf("===== MPU 9250 Demo using Linux =====\n");

  /* ------------------------> Open the I2C adapter <------------------------ */
  int file;
  uint8_t adapter_n = 1;  // For Raspberry Pi 2
  char filename[13];

  // Access an I2C adapter from a C++ program
  snprintf(filename, 13, "/dev/i2c-%d", adapter_n);

  // Open I2C bus driver
  if ((file = open(filename, O_RDWR)) < 0) {
    /* ERROR HANDLING: you can check errno to see what went wrong */
    perror("Failed to open the I2C bus.\n");
    exit(1);
  }

  /* ----------------------> Initiating communication <---------------------- */
  choose_device(file, MPU9250_ADDRESS);

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

  return 0;
}
