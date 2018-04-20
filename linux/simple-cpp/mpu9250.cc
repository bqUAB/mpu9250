// Set of basic functions to access accelerometer, gyroscope, magnetometer,
// and temperature data

#include "mpu9250.h"

// Mpu9250 constructor
Mpu9250::Mpu9250(I2cBus* i2c_n) {
  ptr_i2c = i2c_n;
}

uint8_t Mpu9250::ComTest(uint8_t test_who){
  /* Read the WHO_AM_I register, this is a good test of communication */
  uint8_t who_am_i;

  if (test_who == kWia){
    printf("AK8963 should be: 0x48\t");
    ptr_i2c->ReadFromMem(kAk8963Addr, kWia, &who_am_i);
  } else if (test_who == kWhoAmImpu6500) {
    printf("MPU9250 should be: 0x71\t");
    ptr_i2c->ReadFromMem(kMpu6500Addr, kWhoAmImpu6500, &who_am_i);
  } else {
    perror("Mpu9250 WHO_AM_I register not valid!\n");
    exit(1);
  }

  printf("WHO_AM_I: %#X\n", who_am_i);
  return who_am_i;
}

void Mpu9250::GetGyroRes() {
  /* MPU 9250 Product Specification section 3.1 */
  /* Possible gyro scales (and their register bit settings) are:
   * 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). */
  switch (gyro_scale){
    /* Here's a bit of an algorith to calculate DPS/(ADC tick) based on
     *that 2-bit value: */
    case kGfs250Dps:
      gyro_res = 250.0/32768.0;
      break;
    case kGfs500Dps:
      gyro_res = 500.0/32768.0;
      break;
    case kGfs1000Dps:
      gyro_res = 1000.0/32768.0;
      break;
    case kGfs2000Dps:
      gyro_res = 2000.0/32768.0;
      break;
  }
}

void Mpu9250::GetAccelRes() {
  /* MPU 9250 Product Specification section 3.2 */
  /* Possible accelerometer scales (and their register bit settings) are:
   * 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). */
  switch (accel_scale) {
    /* Here's a bit of an algorith to calculate DPS/(ADC tick) based on that
     * 2-bit value: */
    case kAfs2G:
      accel_res = 2.0/32768.0;
      break;
    case kAfs4G:
      accel_res = 4.0/32768.0;
      break;
    case kAfs8G:
      accel_res = 8.0/32768.0;
      break;
    case kAfs16G:
      accel_res = 16.0/32768.0;
      break;
  }
}

void Mpu9250::GetMagnetomRes() {
  /* Possible magnetometer scales (and their register bit settings) are:
   * 14 bit resolution (0) and 16 bit resolution (1) */
  switch (magnetom_scale) {
    case kMfs14Bits:
      magnetom_res = 10*4912.0/8190.0; // Proper scale to return milliGauss
      break;
    case kMfs16Bits:
      magnetom_res = 10*4912.0/32760.0; // Proper scale to return milliGauss
      break;
  }
}

void Mpu9250::InitMpu9250(){
  /* -------------------> Configure Gyro and Thermometer <------------------- */
  /* Disable FSYNC and set thermometer and gyro bandwith to 41 and 42 Hz
   * rerspectively; minimum delay time for this setting is 5.9 ms, which means
   * sensor fusion update rates cannot be higher than 1 / 0.0059 = 170 Hz
   * DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
   * With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!),
   * 8 kHz, or 1 kHz */
  ptr_i2c->WriteToMem(kMpu6500Addr, kConfig, 0x03);

  /* ------> Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV) <------ */
  /* Use a 200 Hz rate; a rate consistent with the filter update rate determined
   * in config above */
  ptr_i2c->WriteToMem(kMpu6500Addr, kSmplrtDiv, 0x04);

  /* -------------------> Set gyroscope full scale range <------------------- */
  /* Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are
   * left-shifted into positions 4:3 */
  uint8_t c = 0;
  c = c | gyro_scale << 3;  // Set full scale range for the gyro
  ptr_i2c->WriteToMem(kMpu6500Addr, kGyroConfig, c);  // Write new GYRO_CONFIG value to register

  /* Set accelerometer full-scale range configuration */
  c = 0;
  c = c & accel_scale << 3;  // Set full scale range for the accelerometer
  ptr_i2c->WriteToMem(kMpu6500Addr, kAccelConfig, c);  // Write new ACCEL_CONFIG register value

  /* ------------> Set accelerometer sample rate configuration <------------- */
  /* It is possible to get a 4 kHz sample rate from the accelerometer by
   * choosing 1 for accel_fchoice_b bit [3]; in this case the bandwith is
   * 1.13 kHz */
  c = 0;
  c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwith to 41 Hz
  ptr_i2c->WriteToMem(kMpu6500Addr, kAccelConfig2, c);  // Write a new ACCEL_CONFIG2 register value
  /* The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
   * but all these rates are further reduced by a factor of 5 to 200 Hz because
   * of the SMPLRT_DIV setting */

  /* ---------------> Configure Interrupts and Bypass Enable <--------------- */
  /* Set interrupt pin active high, push-pull, hold interrupt pin level HIGH
   * until interrupt cleared, clear on read of INT_STATUS, and enable
   * I2C_BYPASS_EN so additional chips can join the I2C bus and all can be
   * controlled by Linux as master */
  ptr_i2c->WriteToMem(kMpu6500Addr, kIntPinCfg, 0x22);  // Enable magnetometer
  ptr_i2c->WriteToMem(kMpu6500Addr, kIntEnable, 0X01);  // Enable data ready (bit 0) interrupt
  usleep(100*1000);

}

void Mpu9250::ReadAccelData(int16_t* destination){
  uint8_t raw_data[6];  // x/y/z accel register data stored here
  /* Read the six raw data registers into data array */
  ptr_i2c->ReadFromMemInto(kMpu6500Addr, kAccelXoutH, 6, &raw_data[0]);
  /* Turn the MSB and LSB into a signed 16-bit value */
  destination[0] = ((int16_t)raw_data[0] << 8) | raw_data[1];
  destination[1] = ((int16_t)raw_data[2] << 8) | raw_data[3];
  destination[2] = ((int16_t)raw_data[4] << 8) | raw_data[5];
}

void Mpu9250::ReadGyroData(int16_t* destination){
  uint8_t raw_data[6];  // x/y/z gyro register data stored here
  /* Read the six raw data registers sequentially into data array */
  ptr_i2c->ReadFromMemInto(kMpu6500Addr, kGyroXoutH, 6, &raw_data[0]);
  /* Turn the MSB and LSB into a signed 16-bit value */
  destination[0] = ((int16_t)raw_data[0] << 8) | raw_data[1];
  destination[1] = ((int16_t)raw_data[2] << 8) | raw_data[3];
  destination[2] = ((int16_t)raw_data[4] << 8) | raw_data[5];
}

void Mpu9250::ReadMagnetomData(int16_t* destination){
  /* x/y/z gyro register data, ST2 register stored here, must read ST2 at end of
   * data acquisition */
  uint8_t raw_data[7];
  uint8_t data_ready;
  /* Wait for magnetometer data ready bit to be set */
  ptr_i2c->ReadFromMem(kAk8963Addr, kSt1, &data_ready);
  if( data_ready == 0x01)
  {
    /* Read the six raw data and ST2 registers sequentially into data array */
    ptr_i2c->ReadFromMemInto(kAk8963Addr, kHxl, 7, &raw_data[0]);
    uint8_t c = raw_data[6]; // End data read by reading ST2 register
    /* Check if magnetic sensor overflow set, if not then report data */
    if(!(c & 0x08)){
      /* Turn the MSB and LSB into a signed 16-bit value */
      destination[0] = ((int16_t)raw_data[1] << 8) | raw_data[0];
      /* Data stored as little Endian */
      destination[1] = ((int16_t)raw_data[3] << 8) | raw_data[2];
      destination[2] = ((int16_t)raw_data[5] << 8) | raw_data[4];
    }
  }
}

int16_t Mpu9250::ReadTempData() {
  uint8_t raw_data[2];  // temperature register data stored here
  /* Read the two raw data registers sequentially into data array */
  ptr_i2c->ReadFromMemInto(kMpu6500Addr, kTempOutH, 2, &raw_data[0]);
  /* Turn the MSB and LSB into a 16-bit value */
  return ((int16_t)raw_data[0] << 8) | raw_data[1];
}
