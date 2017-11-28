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
* <https://github.com/sparkfun/MPU-9250_Breakout/blob/master/Libraries/Arduino/examples/MPU9250BasicAHRS/MPU9250BasicAHRS.ino>
*******************************************************************************/

#include <stdio.h>    // Needed for printf, snprintf, perror
#include <stdint.h>   // Needed for unit uint8_t data type
#include <stdlib.h>   // Needed for exit()
#include <mpu9250.h>

MPU9250 myIMU;

void loop();

int main(){

  printf("===== MPU 9250 Demo using Linux =====\n");
  myIMU.openI2C();
  /* Initiating communication */
  uint8_t c = myIMU.comTest(WHO_AM_I_MPU9250);

  if (c == 0x71){  // WHO_AM_I should always be 0x71
    printf("MPU9250 is online...\n");

    /* Start by performing self test and reporting values */
    myIMU.MPU9250SelfTest(myIMU.SelfTest);
    printf("Accelerometer Self Test\n");
    printf("x-axis self test: acceleration trim within : ");
    printf("% 0.2f%% of factory value\n", myIMU.SelfTest[0]);
    printf("y-axis self test: acceleration trim within : ");
    printf("% 0.2f%% of factory value\n", myIMU.SelfTest[1]);
    printf("z-axis self test: acceleration trim within : ");
    printf("% 0.2f%% of factory value\n", myIMU.SelfTest[2]);
    printf("Gyroscope Self Test\n");
    printf("x-axis self test: gyration trim within : ");
    printf("% 0.2f%% of factory value\n", myIMU.SelfTest[3]);
    printf("y-axis self test: gyration trim within : ");
    printf("% 0.2f%% of factory value\n", myIMU.SelfTest[4]);
    printf("z-axis self test: gyration trim within : ");
    printf("% 0.2f%% of factory value\n", myIMU.SelfTest[5]);

    /* Calibrate gyro and accelerometer, load biases in bias registers */
    printf("Calibrating MPU9250...\n");
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);

    /* Initialize device for active mode read of accelerometer, gyroscope, and
     * temperature */
    printf("Initializing MPU9250for active data mode...\n");
    myIMU.initMPU9250();

    /* Read the WHO_AM_I register of the magnetometer, this is a good test of
     * communication */
    uint8_t d = myIMU.comTest(WHO_AM_I_AK8963);
    if (d == 0x48){  // WHO_AM_I should always be 0x48
      printf("AK8963 is online...\n");
      printf("Calibrating AK8963...\n");
      /* Get magnetometer calibration from AK8963 ROM */
      myIMU.initAK8963(myIMU.magCalibration);
      printf("AK8963 initialized for active data mode...\n");
      printf("AK8963 calibration values:\n");
      printf("X-Axis sensitivity adjustment value ");
      printf("% 0.2f\n", myIMU.magCalibration[0]);
      printf("Y-Axis sensitivity adjustment value ");
      printf("% 0.2f\n", myIMU.magCalibration[1]);
      printf("Z-Axis sensitivity adjustment value ");
      printf("% 0.2f\n", myIMU.magCalibration[2]);
    } else {
      perror("Could not connect to AK8963");
      exit(1);
    }
  } else {
    perror("Could not connect to MPU9250");
    exit(1);
  }

  loop();

  return 0;
}

void loop(){ while(1){  // Arduino loop like
  /* If intPin goes high, all data registers have new data
   * On interrupt, check if data ready interrupt */
  if (myIMU.readByte(INT_STATUS) & 0x01){
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    myIMU.getAres();

    /* Now we'll calculate the accleration value into actual g's
     * This depends on scale being set */
    myIMU.ax = (float)myIMU.accelCount[0]*myIMU.aRes;  // - accelBias[0];
    myIMU.ay = (float)myIMU.accelCount[1]*myIMU.aRes;  // - accelBias[1];
    myIMU.az = (float)myIMU.accelCount[2]*myIMU.aRes;  // - accelBias[2];
  }
}}
