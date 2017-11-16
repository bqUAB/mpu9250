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
* Created 10 November 2017 based on:
* <http://elinux.org/Interfacing_with_I2C_Devices>
* <https://www.kernel.org/doc/Documentation/i2c/dev-interface>
*******************************************************************************/

#include <stdio.h>          // Needed for printf, snprintf, perror
#include <stdint.h>         // Needed for unit uint8_t data type
#include <stdlib.h>         // Needed for exit()
#include <fcntl.h>          // Needed for open()
#include <sys/ioctl.h>      // Needed for ioctl
#include <linux/i2c-dev.h>  // Needed to use the I2C Linux driver (I2C_SLAVE)
#include <stdbool.h>        // Needed for bool type
#include <unistd.h>         // Needed for write()
#include <mpu9250.h>


bool MPU9250_REG_WRITE(int file, uint8_t reg_add, uint8_t data){
  bool bSuccess = false;
  uint8_t w_buf[2];

  // Write to define register
  w_buf[0] = reg_add;
  w_buf[1] = data;
  if (write(file, &w_buf, sizeof(w_buf)) == sizeof(w_buf)){
    bSuccess = true;
  } else {
    /* ERROR HANDLING: i2c transaction failed */
    printf("Failed to write to the i2c bus.\n");
  }

  return bSuccess;
}

bool MPU9250_REG_READ(int file, uint8_t reg_add, uint8_t *data){
	bool bSuccess = false;
	uint8_t Data;

	// write to define register
	if (write(file, &reg_add, sizeof(reg_add)) == sizeof(reg_add)){

		// read back value
		if (read(file, &Data, sizeof(Data)) == sizeof(Data)){
			*data = Data;
			bSuccess = true;
		}
	}

	return bSuccess;
}

bool MPU9250_REG_MULTI_READ(int file, uint8_t reg_add,\
                            uint8_t count, uint8_t *data){
	bool bSuccess = false;

	// write to define register
	if (write(file, &reg_add, sizeof(reg_add)) == sizeof(reg_add)){
		// read back value
		if (read(file, data, count) == count){
			bSuccess = true;
		}
	}


	return bSuccess;
}

int main(){

  printf("===== MPU 9250 Demo using Linux =====\n");

  /* ------------------------> Open the I2C adapter <------------------------ */
  int file;
  uint8_t adapter_n = 2;  // For BeagleBone Black
  char filename[13];

  // Access an I2C adapter from a C program
  snprintf(filename, 13, "/dev/i2c-%d", adapter_n);

  // Open I2C bus driver
  if ((file = open(filename, O_RDWR)) < 0) {
    /* ERROR HANDLING: you can check errno to see what went wrong */
    perror("Failed to open the I2C bus.\n");
    exit(1);
  }

  /* ----------------------> Initiating communication <---------------------- */
  // Specify device address to communicate
  if (ioctl(file, I2C_SLAVE, MPU9250_ADDRESS) < 0) {
      printf("Failed to acquire bus access and/or talk to slave.\n");
      /* ERROR HANDLING; you can check errno to see what went wrong */
      exit(1);
  }

  uint8_t buf[2]  = {'\0'};  // Buffer to store data with terminating null
  // Read the WHO_AM_I register, this is a good test of communication
  MPU9250_REG_READ(file, WHO_AM_I_MPU9250, buf);
  printf("WHO_AM_I: 0x%X\n", buf[0]);
  printf("I should be: 0x71\n");

  /* ---------------------> Creating MPU9250 structure <--------------------- */
  struct _MPU9250 myIMU;
  // Set initial input parameters
  enum Ascale {
      AFS_2G = 0,
      AFS_4G,
      AFS_8G,
      AFS_16G
    };

  // Specify sensor full scale
  myIMU.Ascale = AFS_2G;

  // Bias corrections for gyro and accelerometer
  for(int i = 0; i < 3; i++){
    myIMU.accelBias[i] = 0;
  }

  printf("myIMU.Ascale = %d\n", myIMU.Ascale);
  for(int i = 0; i < 3; i++){
    printf("myIMU.accelBias[%d] = %.1f\n", i, myIMU.accelBias[i]);
  }

  /* ----------> Start performing self test and reporting values <----------- */
  MPU9250SelfTest(file, myIMU.SelfTest);

  // Accelerometer values
  printf("x-axis self test: acceleration trim within : ");
  printf("%.0f%% of factory value\n", myIMU.SelfTest[0]);

  printf("y-axis self test: acceleration trim within : ");
  printf("%.0f%% of factory value\n", myIMU.SelfTest[1]);

  printf("z-axis self test: acceleration trim within : ");
  printf("%.0f%% of factory value\n", myIMU.SelfTest[2]);

  return 0;
}
