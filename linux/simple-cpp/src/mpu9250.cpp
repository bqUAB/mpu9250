#include <mpu9250.h>

/*******************************************************************************
 * Set of basic functions to access accelerometer, gyroscope, magnetometer, and
 * temperature data
 ******************************************************************************/

void MPU9250::chooseDevice(uint8_t devAdd){
  /* ---------------> Specify device address to communicate <---------------- */
  if (ioctl(*ptrFile, I2C_SLAVE, devAdd) < 0){
    printf("Failed to acquire bus access and/or talk to slave.\n");
    /* ERROR HANDLING; you can check errno to see what went wrong */
    exit(1);
  }
}

/* Linux I2C read and write protocols */
void MPU9250::writeByte(uint8_t regAdd, uint8_t data){

  //uint8_t bSuccess = 0;
  uint8_t wBuf[2];

  /* Write to define register */
  wBuf[0] = regAdd;
  wBuf[1] = data;
  if (write(*ptrFile, &wBuf, sizeof(wBuf)) == sizeof(wBuf)){
    //bSuccess = 1;
  } else {
    /* ERROR HANDLING: i2c transaction failed */
    //bSuccess = 0;
  }
}

uint8_t MPU9250::readByte(uint8_t regAdd){

  //uint8_t bSuccess = 0;
  uint8_t data; // `data` will store the register data

  /* write to define register */
  if (write(*ptrFile, &regAdd, sizeof(regAdd)) == sizeof(regAdd)){

    /* read back value */
    if (read(*ptrFile, &data, sizeof(data)) == sizeof(data)){
      //bSuccess = 1;
    }
  }

  return data;  // Return data read from slave register
}

void MPU9250::readBytes(uint8_t regAdd, uint8_t count,
                        uint8_t* data){

  //uint8_t bSuccess = 0;
  /* write to define register */
  if (write(*ptrFile, &regAdd, sizeof(regAdd)) == sizeof(regAdd)){
    // read back value
    if (read(*ptrFile, data, count) == count){
      //bSuccess = 1;
    }
  }
}

uint8_t MPU9250::comTest(uint8_t WHO_AM_I){
  /* Read the WHO_AM_I register, this is a good test of communication */
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

void MPU9250::readAccelData(int16_t* destination){
  chooseDevice(MPU9250_ADDRESS);
  uint8_t rawData[6];  // x/y/z accel register data stored here
  /* Read the six raw data registers into data array */
  readBytes(ACCEL_XOUT_H, 6, &rawData[0]);
  /* Turn the MSB and LSB into a signed 16-bit value */
  destination[0] = ((int16_t)rawData[0] << 8) | rawData[1];
  destination[1] = ((int16_t)rawData[2] << 8) | rawData[3];
  destination[2] = ((int16_t)rawData[4] << 8) | rawData[5];
}

void MPU9250::readGyroData(int16_t* destination){
  chooseDevice(MPU9250_ADDRESS);
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  /* Read the six raw data registers sequentially into data array */
  readBytes(GYRO_XOUT_H, 6, &rawData[0]);
  /* Turn the MSB and LSB into a signed 16-bit value */
  destination[0] = ((int16_t)rawData[0] << 8) | rawData[1];
  destination[1] = ((int16_t)rawData[2] << 8) | rawData[3];
  destination[2] = ((int16_t)rawData[4] << 8) | rawData[5];
}

void MPU9250::readMagData(int16_t* destination){
  chooseDevice(AK8963_ADDRESS);
  /* x/y/z gyro register data, ST2 register stored here, must read ST2 at end of
   * data acquisition */
  uint8_t rawData[7];
  /* Wait for magnetometer data ready bit to be set */
  if(readByte(AK8963_ST1) & 0x01)
  {
    /* Read the six raw data and ST2 registers sequentially into data array */
    readBytes(AK8963_XOUT_L, 7, &rawData[0]);
    uint8_t c = rawData[6]; // End data read by reading ST2 register
    /* Check if magnetic sensor overflow set, if not then report data */
    if(!(c & 0x08)){
      /* Turn the MSB and LSB into a signed 16-bit value */
      destination[0] = ((int16_t)rawData[1] << 8) | rawData[0];
      /* Data stored as little Endian */
      destination[1] = ((int16_t)rawData[3] << 8) | rawData[2];
      destination[2] = ((int16_t)rawData[5] << 8) | rawData[4];
    }
  }
}

void MPU9250::getAres() {
  /* Possible accelerometer scales (and their register bit settings) are:
   * 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). */
  switch (Ascale){
    /* Here's a bit of an algorith to calculate DPS/(ADC tick) based on that
     * 2-bit value: */
    case AFS_2G:
      aRes = 2.0/32768.0;
      break;
    case AFS_4G:
      aRes = 4.0/32768.0;
      break;
    case AFS_8G:
      aRes = 8.0/32768.0;
      break;
    case AFS_16G:
      aRes = 16.0/32768.0;
      break;
  }
}

void MPU9250::getGres() {
  /* Possible gyro scales (and their register bit settings) are:
   * 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). */
  switch (Gscale){
    /* Here's a bit of an algorith to calculate DPS/(ADC tick) based on
     *that 2-bit value: */
    case GFS_250DPS:
      gRes = 250.0/32768.0;
      break;
    case GFS_500DPS:
      gRes = 500.0/32768.0;
      break;
    case GFS_1000DPS:
      gRes = 1000.0/32768.0;
      break;
    case GFS_2000DPS:
      gRes = 2000.0/32768.0;
      break;
  }
}

void MPU9250::getMres() {
  /* Possible magnetometer scales (and their register bit settings) are:
   * 14 bit resolution (0) and 16 bit resolution (1) */
  switch (Mscale){
    case MFS_14BITS:
      mRes = 10.*4912./8190.; // Proper scale to return milliGauss
      break;
    case MFS_16BITS:
      mRes = 10.*4912./32760.0; // Proper scale to return milliGauss
      break;
  }
}

int16_t MPU9250::readTempData() {
  chooseDevice(MPU9250_ADDRESS);
  uint8_t rawData[2];  // x/y/z gyro register data stored here
  /* Read the two raw data registers sequentially into data array */
  readBytes(TEMP_OUT_H, 2, &rawData[0]);
  /* Turn the MSB and LSB into a 16-bit value */
  return ((int16_t)rawData[0] << 8) | rawData[1];
}
