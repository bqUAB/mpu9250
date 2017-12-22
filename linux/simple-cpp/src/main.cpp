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
void setup();
void loop();

MPU9250 myIMU;

int main(){
  /* Variable declaration */
  int file = 0;

  printf("===== MPU 9250 Demo using Linux =====\n");
  openI2C(&file, 1);
  myIMU.ptrFile = &file;

  setup();
  loop();

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

void setup() {  // Arduino setup like
  /* Initiating communication */
  uint8_t c = myIMU.comTest(WHO_AM_I_MPU9250);

  if (c == 0x71){  // WHO_AM_I should always be 0x71
    printf("MPU9250 is online...\n");

    myIMU.getGres();
    myIMU.getAres();

    /* Read the WHO_AM_I register of the magnetometer, this is a good test of
     * communication */
    uint8_t d = myIMU.comTest(WHO_AM_I_AK8963);
    if (d == 0x48){  // WHO_AM_I should always be 0x48
      printf("AK8963 is online...\n");
      myIMU.getMres();
    } else {
      perror("Could not connect to AK8963");
      exit(1);
    }
  } else {
    perror("Could not connect to MPU9250");
    exit(1);
  }
}

void loop() { while(1){  // Arduino loop like
  /* If intPin goes high, all data registers have new data
   * On interrupt, check if data ready interrupt */
  if (myIMU.readByte(INT_STATUS) & 0x01){
    
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    /* Now we'll calculate the acceleration value into actual g's
     * This depends on scale being set */
    myIMU.ax = (float)myIMU.accelCount[0]*myIMU.aRes;
    myIMU.ay = (float)myIMU.accelCount[1]*myIMU.aRes;
    myIMU.az = (float)myIMU.accelCount[2]*myIMU.aRes;

    myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
    /* Calculate the gyro value into actual degrees per second
     * This depends on scale being set */
    myIMU.gx = (float)myIMU.gyroCount[0]*myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1]*myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2]*myIMU.gRes;

    myIMU.readMagData(myIMU.magCount);  // Read the x/y/z adc values
    /* User environmental x-axis correction in milliGauss, should be
     * automatically calculated */
    myIMU.magbias[0] = +470.;
    // User environmental y-axis correction in milliGauss TODO axis??
    myIMU.magbias[1] = +120.;
    // User environmental z-axis correction in milliGauss
    myIMU.magbias[2] = +125.;

    /* Calculate the magnetometer values in milliGauss
     * Include factory calibration per data sheet and user environmental
     * corrections
     * Get actual magnetometer value, this depends on scale being set */
    myIMU.mx = (float)myIMU.magCount[0]*myIMU.mRes*myIMU.magCalibration[0] -
              myIMU.magbias[0];
    myIMU.my = (float)myIMU.magCount[1]*myIMU.mRes*myIMU.magCalibration[1] -
              myIMU.magbias[1];
    myIMU.mz = (float)myIMU.magCount[2]*myIMU.mRes*myIMU.magCalibration[2] -
              myIMU.magbias[2];

    myIMU.tempCount = myIMU.readTempData();  // Read the adc values
  }

  /* Print acceleration values in milligs! */
  printf("X-acceleration: % 0.2f mg\n", 1000*myIMU.ax);
  printf("Y-acceleration: % 0.2f mg\n", 1000*myIMU.ay);
  printf("Z-acceleration: % 0.2f mg\n", 1000*myIMU.az);

  /* Print gyro values in degree/sec */
  printf("X-gyro rate: % 0.2f degrees/sec\n", myIMU.gx);
  printf("Y-gyro rate: % 0.2f degrees/sec\n", myIMU.gy);
  printf("Z-gyro rate: % 0.2f degrees/sec\n", myIMU.gz);

  /* Print mag values in degree/sec */
  printf("X-mag field: % 0.2f mG\n", myIMU.mx);
  printf("Y-mag field: % 0.2f mG\n", myIMU.my);
  printf("Z-mag field: % 0.2f mG\n", myIMU.mz);

  /* Temperature in degrees Centigrade */
  /* TEMP_degC = ((TEMP_OUT - RoomTemp_Offset)/Temp_Sensitivity) + 21degC 
   * Data found on MPU-9250 Product Specification section 3.4.2
   * Sensitivity = 333.87 LSB/°C 
   * Room Temp Offset = 0 LSB */
  myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;
  // Print temperature in degrees Centigrade
  printf("Temperature is % 0.2f degrees C\n", myIMU.temperature);

  usleep(0.2*1000000);
}}
