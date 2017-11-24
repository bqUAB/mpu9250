#include <mpu9250.h>

// Linux I2C read and write protocols
void MPU9250::writeByte(int file, uint8_t reg_add, uint8_t data){

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

uint8_t MPU9250::readByte(int file, uint8_t reg_add){

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

void MPU9250::readBytes(int file, uint8_t reg_add, uint8_t count,
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
