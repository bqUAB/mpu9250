#include <mpu9250.h>

void MPU9250::open_i2c(){
  /* ------------------------> Open the I2C adapter <------------------------ */
  uint8_t adapter_n = 1;  // For Raspberry Pi 2
  char filename[11];      // To hold /dev/i2c-#

  // Access an I2C adapter from a C++ program
  snprintf(filename, sizeof(filename), "/dev/i2c-%d", adapter_n);

  // Open I2C bus driver
  if ((file = open(filename, O_RDWR)) < 0) {
    /* ERROR HANDLING: you can check errno to see what went wrong */
    perror("Failed to open the I2C bus.\n");
    exit(1);
  }
}

void MPU9250::choose_device(uint8_t dev_add){
  /* ----------------> Specify device address to communicate <--------------- */
  if (ioctl(file, I2C_SLAVE, dev_add) < 0){
    printf("Failed to acquire bus access and/or talk to slave.\n");
    /* ERROR HANDLING; you can check errno to see what went wrong */
    exit(1);
  }
}

// Linux I2C read and write protocols
void MPU9250::writeByte(uint8_t reg_add, uint8_t data){

  //uint8_t bSuccess = 0;
  uint8_t w_buf[2];

  // Write to define register
  w_buf[0] = reg_add;
  w_buf[1] = data;
  if (write(file, &w_buf, sizeof(w_buf)) == sizeof(w_buf)){
    //bSuccess = 1;
  } else {
    /* ERROR HANDLING: i2c transaction failed */
    //bSuccess = 0;
  }
}

uint8_t MPU9250::readByte(uint8_t reg_add){

  //uint8_t bSuccess = 0;
  uint8_t data; // `data` will store the register data

  // write to define register
  if (write(file, &reg_add, sizeof(reg_add)) == sizeof(reg_add)){

    // read back value
    if (read(file, &data, sizeof(data)) == sizeof(data)){
      //bSuccess = 1;
    }
  }

  return data;  // Return data read from slave register
}

void MPU9250::readBytes(uint8_t reg_add, uint8_t count,
                        uint8_t * data){

  //uint8_t bSuccess = 0;
  // write to define register
  if (write(file, &reg_add, sizeof(reg_add)) == sizeof(reg_add)){
    // read back value
    if (read(file, data, count) == count){
      //bSuccess = 1;
    }
  }
}

void MPU9250SelfTest(float * destination){

}
