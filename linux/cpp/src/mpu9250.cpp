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
