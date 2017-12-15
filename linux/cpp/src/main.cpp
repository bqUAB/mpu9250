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
#include <fcntl.h>          // Needed for open()
#include <unistd.h>         // Needed for close()
#include <stdlib.h>         // Needed for exit()
#include <sys/ioctl.h>      // Needed for ioctl
#include <linux/i2c-dev.h>  // Needed to use the I2C Linux driver (I2C_SLAVE)
#include <mpu9250.h>

void openI2C(int* ptrFile, uint8_t adapterN);
void chooseDevice(int* ptrFile, uint8_t devAdd);

MPU9250 myIMU;

int main(){
  /* Variable declaration */
  int file = 0;

  printf("===== MPU 9250 Demo using Linux =====\n");
  openI2C(&file, 1);
  myIMU.ptrFile = &file;
  uint8_t c = myIMU.comTest(WHO_AM_I_MPU9250);

  if (c == 0x71){  // WHO_AM_I should always be 0x71
    printf("MPU9250 is online...\n");
  }

  return 0;
}

void openI2C(int* ptrFile, uint8_t adapterN){
  /* ------------------------> Open the I2C adapter <------------------------ */
  char filename[11]; // To hold /dev/i2c-#

  /* Access an I2C adapter from a C++ program */
  snprintf(filename, sizeof(filename), "/dev/i2c-%d", adapterN);

  /* Open I2C bus driver */
  if ((*ptrFile = open(filename, O_RDWR)) < 0) {
    /* ERROR HANDLING: you can check errno to see what went wrong */
    perror("Failed to open the I2C bus.\n");
    exit(1);
  }
}

void chooseDevice(int* ptrFile, uint8_t devAdd){
  /* ---------------> Specify device address to communicate <---------------- */
  if (ioctl(*ptrFile, I2C_SLAVE, devAdd) < 0){
    printf("Failed to acquire bus access and/or talk to slave.\n");
    /* ERROR HANDLING; you can check errno to see what went wrong */
    exit(1);
  }
}
