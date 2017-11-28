#include <mpu9250.h>

void MPU9250::openI2C(){
  /* ------------------------> Open the I2C adapter <------------------------ */
  uint8_t adapterN = 1;  // For Raspberry Pi 2
  char filename[11];     // To hold /dev/i2c-#

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

/* Accelerometer and gyroscope self test; check calibration wrt factory
  settings. Should return percent deviation from factory trim values, +/- 14 or
  less deviation is a pass */
void MPU9250::MPU9250SelfTest(float * destination){
  chooseDevice(MPU9250_ADDRESS);
  uint8_t rawData[6] = {0, 0, 0, 0, 0, 0};
  uint8_t selfTest[6];
  int16_t gAvg[3], gSTAvg[3], aAvg[3], aSTAvg[3];
  float factoryTrim[6];
  uint8_t FS = 0;

  // Set gyro sample rate to 1 kHz
  writeByte(SMPLRT_DIV,    0x00);
  // Set gyro sample rate to 1 kHz and DLPF to 92 Hz
  writeByte(CONFIG,        0x02);
  // Set full scale range for the gyro to 250 dps
  writeByte(GYRO_CONFIG,   1<<FS);
  // Set accelerometer rate to 1 kHz and bandwidth to 92 Hz
  writeByte(ACCEL_CONFIG2, 0x02);
  // Set full scale range for the accelerometer to 2 g
  writeByte(ACCEL_CONFIG,  1<<FS);

  /* get average current values of gyro and acclerometer */
  for(int i = 0; i < 200; i++) {

    // Read the six raw data registers into data array
    readBytes(ACCEL_XOUT_H, 6, &rawData[0]);
    // Turn the MSB and LSB into a signed 16-bit value
    aAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);
    aAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);
    aAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]);

    // Read the six raw data registers sequentially into data array
    readBytes(GYRO_XOUT_H, 6, &rawData[0]);
    // Turn the MSB and LSB into a signed 16-bit value
    gAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);
    gAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);
    gAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]);
  }

  // Get average of 200 values and store as average current readings
  for(int i =0; i < 3; i++) {
    aAvg[i] /= 200;
    gAvg[i] /= 200;
  }

  /* Configure the gyroscope and accelerometer for self-test */
  // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
  writeByte(GYRO_CONFIG,  0xE0);
  // Enable self test on all three axes and set accelerometer range to +/- 2 g
  writeByte(ACCEL_CONFIG, 0xE0);
  usleep(25*1000);  // Delay a while to let the device stabilize

  /* get average self-test values of gyro and acclerometer */
  for( int i = 0; i < 200; i++) {

    // Read the six raw data registers into data array
    readBytes(ACCEL_XOUT_H, 6, &rawData[0]);
    // Turn the MSB and LSB into a signed 16-bit value
    aSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);
    aSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);
    aSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]);

    // Read the six raw data registers sequentially into data array
    readBytes(GYRO_XOUT_H, 6, &rawData[0]);
    // Turn the MSB and LSB into a signed 16-bit value
    gSTAvg[0] += (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]);
    gSTAvg[1] += (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]);
    gSTAvg[2] += (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]);
  }

  // Get average of 200 values and store as average self-test readings
  for (int i =0; i < 3; i++) {
    aSTAvg[i] /= 200;
    gSTAvg[i] /= 200;
  }

  // Configure the gyro and accelerometer for normal operation
  writeByte(GYRO_CONFIG,  0x00);
  writeByte(ACCEL_CONFIG, 0x00);
  usleep(25*1000);  // Delay a while to let the device stabilize

  // Retrieve accelerometer and gyro factory Self-Test Code from USR_Reg
  selfTest[0] = readByte(SELF_TEST_X_ACCEL); // X-axis accel self-test results
  selfTest[1] = readByte(SELF_TEST_Y_ACCEL); // Y-axis accel self-test results
  selfTest[2] = readByte(SELF_TEST_Z_ACCEL); // Z-axis accel self-test results
  selfTest[3] = readByte(SELF_TEST_X_GYRO);  // X-axis gyro self-test results
  selfTest[4] = readByte(SELF_TEST_Y_GYRO);  // Y-axis gyro self-test results
  selfTest[5] = readByte(SELF_TEST_Z_GYRO);  // Z-axis gyro self-test results

  /* Retrieve factory self-test value from self-test code reads */
  // FT[Xa] factory trim calculation
  factoryTrim[0] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[0] - 1.0)));
  // FT[Ya] factory trim calculation
  factoryTrim[1] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[1] - 1.0)));
  // FT[Za] factory trim calculation
  factoryTrim[2] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[2] - 1.0)));
  // FT[Xg] factory trim calculation
  factoryTrim[3] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[3] - 1.0)));
  // FT[Yg] factory trim calculation
  factoryTrim[4] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[4] - 1.0)));
  // FT[Zg] factory trim calculation
  factoryTrim[5] = (float)(2620/1<<FS)*(pow(1.01, ((float)selfTest[5] - 1.0)));

  // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of
  // the Self-Test Response. To get percent, must multiply by 100
  for (int i = 0; i < 3; i++) {
    // Report percent differences
    destination[i]   = 100.0*((float)(aSTAvg[i] - aAvg[i]))/factoryTrim[i];
    destination[i+3] = 100.0*((float)(gSTAvg[i] - gAvg[i]))/factoryTrim[i+3];
  }
}

// Function which accumulates gyro and accelerometer data after device
// initialization. It calculates the average of the at-rest readings and then
// loads the resulting offsets into accelerometer and gyro bias registers.
void MPU9250::calibrateMPU9250(float * gyroBias, float * accelBias){
  chooseDevice(MPU9250_ADDRESS);
  uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
  uint16_t i, packet_count, fifo_count;
  int32_t gyro_bias[3]  = {0, 0, 0}, accel_bias[3] = {0, 0, 0};

  /* reset device */
  // Write a one to bit 7 reset bit; toggle reset device
  writeByte(PWR_MGMT_1, 0x80);
  usleep(100*1000);

  /* get stable time source; Auto select clock source to be PLL gyroscope
     reference if ready else use the internal oscillator, bits 2:0 = 001 */
  writeByte(PWR_MGMT_1, 0x01);
  writeByte(PWR_MGMT_2, 0x00);
  usleep(200*1000);

  /* Configure device for bias calculation */
  writeByte(INT_ENABLE,   0x00);  // Disable all interrupts
  writeByte(FIFO_EN,      0x00);  // Disable FIFO
  writeByte(PWR_MGMT_1,   0x00);  // Turn on internal clock source
  writeByte(I2C_MST_CTRL, 0x00);  // Disable I2C master
  writeByte(USER_CTRL,    0x00);  // Disable FIFO and I2C master modes
  writeByte(USER_CTRL,    0x0C);  // Reset FIFO and DMP
  usleep(15*1000);

  /* Configure MPU6050 gyro and accelerometer for bias calculation */
  writeByte(CONFIG,       0x01);  // Set low-pass filter to 188 Hz
  writeByte(SMPLRT_DIV,   0x00);  // Set sample rate to 1 kHz
  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
  writeByte(GYRO_CONFIG,  0x00);
  // Set accelerometer full-scale to 2 g, maximum sensitivity
  writeByte(ACCEL_CONFIG, 0x00);

  uint16_t  gyrosensitivity  = 131;    // = 131 LSB/degrees/sec
  uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

  // Configure FIFO to capture accelerometer and gyro data for bias calculation
  writeByte(USER_CTRL, 0x40);  // Enable FIFO
  // Enable gyro and accelerometer sensors for FIFO
  writeByte(FIFO_EN, 0x78);  // (max size 512 bytes in MPU-9150)
  usleep(40*1000); // accumulate 40 samples in 40 milliseconds = 480 bytes

  // At end of sample accumulation, turn off FIFO sensor read
  writeByte(FIFO_EN, 0x00);  // Disable gyro and accelerometer sensors for FIFO
  readBytes(FIFO_COUNTH, 2, &data[0]);  // read FIFO sample count
  fifo_count = ((uint16_t)data[0] << 8) | data[1];
  // How many sets of full gyro and accelerometer data for averaging
  packet_count = fifo_count/12;

  for (i = 0; i < packet_count; i++)  {
    int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
    readBytes(FIFO_R_W, 12, &data[0]); // read data for averaging
    // Form signed 16-bit integer for each sample in FIFO
    accel_temp[0] = (int16_t) (((int16_t)data[0]  << 8) |  data[1]);
    accel_temp[1] = (int16_t) (((int16_t)data[2]  << 8) |  data[3]);
    accel_temp[2] = (int16_t) (((int16_t)data[4]  << 8) |  data[5]);
    gyro_temp[0]  = (int16_t) (((int16_t)data[6]  << 8) |  data[7]);
    gyro_temp[1]  = (int16_t) (((int16_t)data[8]  << 8) |  data[9]);
    gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) |  data[11]);

    // Sum individual signed 16-bit biases to get accumulated signed 32-bit
    // biases
    accel_bias[0] += (int32_t) accel_temp[0];
    accel_bias[1] += (int32_t) accel_temp[1];
    accel_bias[2] += (int32_t) accel_temp[2];
    gyro_bias[0]  += (int32_t) gyro_temp[0];
    gyro_bias[1]  += (int32_t) gyro_temp[1];
    gyro_bias[2]  += (int32_t) gyro_temp[2];
  }

  // Normalize sums to get average count biases
  accel_bias[0] /= (int32_t) packet_count;
  accel_bias[1] /= (int32_t) packet_count;
  accel_bias[2] /= (int32_t) packet_count;
  gyro_bias[0]  /= (int32_t) packet_count;
  gyro_bias[1]  /= (int32_t) packet_count;
  gyro_bias[2]  /= (int32_t) packet_count;

  // Remove gravity from the z-axis accelerometer bias calculation
  if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}
  else {accel_bias[2] += (int32_t) accelsensitivity;}

  /* Construct the gyro biases for push to the hardware gyro bias registers,
     which are reset to zero upon device startup */
  // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input
  // format
  data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF;
  // Biases are additive, so change sign on calculated average gyro biases
  data[1] = (-gyro_bias[0]/4)       & 0xFF;
  data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
  data[3] = (-gyro_bias[1]/4)       & 0xFF;
  data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
  data[5] = (-gyro_bias[2]/4)       & 0xFF;

  // Push gyro biases to hardware registers
  writeByte(XG_OFFSET_H, data[0]);
  writeByte(XG_OFFSET_L, data[1]);
  writeByte(YG_OFFSET_H, data[2]);
  writeByte(YG_OFFSET_L, data[3]);
  writeByte(ZG_OFFSET_H, data[4]);
  writeByte(ZG_OFFSET_L, data[5]);

  // Output scaled gyro biases for display in the main program
  gyroBias[0] = (float) gyro_bias[0]/(float) gyrosensitivity;
  gyroBias[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
  gyroBias[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

  /* Construct the accelerometer biases for push to the hardware accelerometer
     bias registers. These registers contain factory trim values which must be
     added to the calculated accelerometer biases; on boot up these registers
     will hold non-zero values. In addition, bit 0 of the lower byte must be
     preserved since it is used for temperature compensation calculations.
     Accelerometer bias registers expect bias input as 2048 LSB per g, so that
     the accelerometer biases calculated above must be divided by 8. */
  // A place to hold the factory accelerometer trim biases
  int32_t accel_bias_reg[3] = {0, 0, 0};
  readBytes(XA_OFFSET_H, 2, &data[0]); // Read factory accelerometer trim values
  accel_bias_reg[0] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
  readBytes(YA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[1] = (int32_t) (((int16_t)data[0] << 8) | data[1]);
  readBytes(ZA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[2] = (int32_t) (((int16_t)data[0] << 8) | data[1]);

  // Define mask for temperature compensation bit 0 of lower byte of
  // accelerometer bias registers
  uint32_t mask = 1uL;
  // Define array to hold mask bit for each accelerometer bias axis
  uint8_t mask_bit[3] = {0, 0, 0};

  for(i = 0; i < 3; i++) {
      // If temperature compensation bit is set, record that fact in mask_bit
      if((accel_bias_reg[i] & mask)) mask_bit[i] = 0x01;
  }

  /* Construct total accelerometer bias, including calculated average
     accelerometer bias from above */
  // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g
  // (16 g full scale)
  accel_bias_reg[0] -= (accel_bias[0]/8);
  accel_bias_reg[1] -= (accel_bias[1]/8);
  accel_bias_reg[2] -= (accel_bias[2]/8);

  data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
  data[1] = (accel_bias_reg[0])      & 0xFF;
  // preserve temperature compensation bit when writing back to accelerometer
  // bias registers
  data[1] = data[1] | mask_bit[0];
  data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
  data[3] = (accel_bias_reg[1])      & 0xFF;
  // preserve temperature compensation bit when writing back to accelerometer
  // bias registers
  data[3] = data[3] | mask_bit[1];
  data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
  data[5] = (accel_bias_reg[2])      & 0xFF;
  // preserve temperature compensation bit when writing back to accelerometer
  // bias registers
  data[5] = data[5] | mask_bit[2];

  // Apparently this is not working for the acceleration biases in the MPU-9250
  // Are we handling the temperature correction bit properly?
  // Push accelerometer biases to hardware registers
  writeByte(XA_OFFSET_H, data[0]);
  writeByte(XA_OFFSET_L, data[1]);
  writeByte(YA_OFFSET_H, data[2]);
  writeByte(YA_OFFSET_L, data[3]);
  writeByte(ZA_OFFSET_H, data[4]);
  writeByte(ZA_OFFSET_L, data[5]);

  // Output scaled accelerometer biases for display in the main program
  accelBias[0] = (float)accel_bias[0]/(float)accelsensitivity;
  accelBias[1] = (float)accel_bias[1]/(float)accelsensitivity;
  accelBias[2] = (float)accel_bias[2]/(float)accelsensitivity;
}

void MPU9250::initMPU9250(){
  chooseDevice(MPU9250_ADDRESS);

  /* Wake up device */
  writeByte(PWR_MGMT_1, 0x00);  // Clear sleep mode bit (6), enable all sensors
  usleep(100*1000);             // Wait for all registers to reset

  /* get stable time source */
  // Auto selects the best available clock source – PLL if ready, else
  // use the Internal oscillator
  writeByte(PWR_MGMT_1, 0x01);
  usleep(200*1000);

  /* Configure Gyro and Thermometer */
  // Disable FSYNC and set thermometer and gyro bandwith to 41 and 42 Hz
  // rerspectively; minimum delay time for this setting is 5.9 ms, which means
  // sensor fusion update rates cannot be higher than 1 / 0.0059 = 170 Hz
  // DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
  // With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!),
  // 8 kHz, or 1 kHz
  writeByte(CONFIG, 0x03);

  /* Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV) */
  // Use a 200 Hz rate; a rate consistent with the filter update rate determined
  // in config above
  writeByte(SMPLRT_DIV, 0x04);

  /* Set gyroscope full scale range */
  // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are
  // left-shifted into positions 4:3
  uint8_t c = readByte(GYRO_CONFIG);  // get current GYRO_CONFIG register value
  // c = c & ~0xE0;  // Clear self-test bits [7:5]
  c = c & ~0x02;  // Clear Fchoice bits [1:0]
  c = c & ~0x18;  // Clear AFS bits [4:3]
  c = c | Gscale << 3;  // Set full scale range for the gyro
  // c =| 0x00;  // Set Fchoice for the gyro to 11 by writing its inverse to
                 //bits 1:0 of GYRO_CONFIG
  writeByte(GYRO_CONFIG, c);  // Write new ACCEL_CONFIG2 register value

  /* Set accelerometer full-scale range configuration */
  c = readByte(ACCEL_CONFIG); // Get current ACCEL_CONFIG register value
  // c = c & ~0xE0;  // Clear self-test bits [7:5]
  c = c & ~0x18;  // Clear AFS bits [4:3]
  c = c & Ascale << 3;  // Set full scale range for the accelerometer
  writeByte(ACCEL_CONFIG, c);  // Write new ACCEL_CONFIG register value

  /* Set accelerometer sample rate configuration */
  // It is possible to get a 4 kHz sample rate from the accelerometer by
  // choosing 1 for accel_fchoice_b bit [3]; in this case the bandwith is
  // 1.13 kHz
  c = readByte(ACCEL_CONFIG2);  // get current ACCEL_CONFIG2 register value
  c = c & ~0x0F;  // Clear accel_fchoice_b (bit 3) and A_DLPFG (bits [2:0])
  c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwith to 41 Hz
  writeByte(ACCEL_CONFIG2, c);  // Write a new ACCEL_CONFIG2 register value
  // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
  // but all these rates are further reduced by a factor of 5 to 200 Hz because
  // of the SMPLRT_DIV setting

  /* Configure Interrupts and Bypass Enable */
  // Set interrupt pin active high, push-pull, hold interrupt pin level HIGH
  // until interrupt cleared, clear on read of INT_STATUS, and enable
  // I2C_BYPASS_EN so additional chips can join the I2C bus and all can be
  // controlled by Linux as master
  writeByte(INT_PIN_CFG, 0x22);
  writeByte(INT_ENABLE, 0X01);  // Enable data ready (bit 0) interrupt
  usleep(100*1000);

}
