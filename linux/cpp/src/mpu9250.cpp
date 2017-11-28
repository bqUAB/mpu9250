#include <mpu9250.h>

void MPU9250::openI2C(){
  /* ------------------------> Open the I2C adapter <------------------------ */
  uint8_t adapterN = 1;  // For Raspberry Pi 2
  char filename[11];      // To hold /dev/i2c-#

  // Access an I2C adapter from a C++ program
  snprintf(filename, sizeof(filename), "/dev/i2c-%d", adapterN);

  // Open I2C bus driver
  if ((file = open(filename, O_RDWR)) < 0) {
    /* ERROR HANDLING: you can check errno to see what went wrong */
    perror("Failed to open the I2C bus.\n");
    exit(1);
  }
}

void MPU9250::chooseDevice(uint8_t devAdd){
  /* ----------------> Specify device address to communicate <--------------- */
  if (ioctl(file, I2C_SLAVE, devAdd) < 0){
    printf("Failed to acquire bus access and/or talk to slave.\n");
    /* ERROR HANDLING; you can check errno to see what went wrong */
    exit(1);
  }
}

// Linux I2C read and write protocols
void MPU9250::writeByte(uint8_t regAdd, uint8_t data){

  //uint8_t bSuccess = 0;
  uint8_t wBuf[2];

  // Write to define register
  wBuf[0] = regAdd;
  wBuf[1] = data;
  if (write(file, &wBuf, sizeof(wBuf)) == sizeof(wBuf)){
    //bSuccess = 1;
  } else {
    /* ERROR HANDLING: i2c transaction failed */
    //bSuccess = 0;
  }
}

uint8_t MPU9250::readByte(uint8_t regAdd){

  //uint8_t bSuccess = 0;
  uint8_t data; // `data` will store the register data

  // write to define register
  if (write(file, &regAdd, sizeof(regAdd)) == sizeof(regAdd)){

    // read back value
    if (read(file, &data, sizeof(data)) == sizeof(data)){
      //bSuccess = 1;
    }
  }

  return data;  // Return data read from slave register
}

void MPU9250::readBytes(uint8_t regAdd, uint8_t count,
                        uint8_t * data){

  //uint8_t bSuccess = 0;
  // write to define register
  if (write(file, &regAdd, sizeof(regAdd)) == sizeof(regAdd)){
    // read back value
    if (read(file, data, count) == count){
      //bSuccess = 1;
    }
  }
}

uint8_t MPU9250::comTest(uint8_t WHO_AM_I){
  // Read the WHO_AM_I register, this is a good test of communication
  uint8_t c;

  if (WHO_AM_I == WHO_AM_I_AK8963){
    chooseDevice(AK8963_ADDRESS);
    printf("AK8963 should be: 0x48\t");
    c = readByte(WHO_AM_I_AK8963);
  } else {
    chooseDevice(MPU9250_ADDRESS);
    printf("MPU9250 should be: 0x71\t");
    c = readByte(WHO_AM_I_MPU9250);
  }

  printf("WHO_AM_I: %#X\n", c);
  return c;
}

void MPU9250::initMPU9250(){
  chooseDevice(MPU9250_ADDRESS);

  // Wake up device
  writeByte(PWR_MGMT_1, 0x00);  // Clear sleep mode bit (6), enable all sensors
  usleep(100*1000);             // Wait for all registers to reset

  // get stable time source

}
