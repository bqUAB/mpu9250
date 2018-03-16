// Set of basic functions to access accelerometer, gyroscope, magnetometer, and
// temperature data

#include "mpu9250.h"


uint8_t Mpu9250::ComTest(uint8_t who_am_i){
  /* Read the WHO_AM_I register, this is a good test of communication */
  uint8_t c;

  if (who_am_i == kAk8963Addr){
    printf("AK8963 should be: 0x48\t");
    c = i2c_n.ReadFromMem(kAk8963Addr, kWia);
  } else {
    printf("MPU9250 should be: 0x71\t");
    c = i2c_n.ReadFromMem(kMpu6500Addr, kWhoAmImpu6500);
  }

  printf("WHO_AM_I: %#X\n", c);
  return c;
}

void Mpu9250::InitMPU9250(){
  /* -------------------> Configure Gyro and Thermometer <------------------- */
  /* Disable FSYNC and set thermometer and gyro bandwith to 41 and 42 Hz
   * rerspectively; minimum delay time for this setting is 5.9 ms, which means
   * sensor fusion update rates cannot be higher than 1 / 0.0059 = 170 Hz
   * DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both
   * With the MPU9250, it is possible to get gyro sample rates of 32 kHz (!),
   * 8 kHz, or 1 kHz */
  writeByte(CONFIG, 0x03);

  /* ------> Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV) <------ */
  /* Use a 200 Hz rate; a rate consistent with the filter update rate determined
   * in config above */
  writeByte(SMPLRT_DIV, 0x04);

  /* -------------------> Set gyroscope full scale range <------------------- */
  /* Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are
   * left-shifted into positions 4:3 */
  uint8_t c = 0;
  c = c | Gscale << 3;  // Set full scale range for the gyro
  writeByte(GYRO_CONFIG, c);  // Write new GYRO_CONFIG value to register

  /* Set accelerometer full-scale range configuration */
  c = 0;
  c = c & Ascale << 3;  // Set full scale range for the accelerometer
  writeByte(ACCEL_CONFIG, c);  // Write new ACCEL_CONFIG register value

  /* ------------> Set accelerometer sample rate configuration <------------- */
  /* It is possible to get a 4 kHz sample rate from the accelerometer by
   * choosing 1 for accel_fchoice_b bit [3]; in this case the bandwith is
   * 1.13 kHz */
  c = 0;
  c = c | 0x03;  // Set accelerometer rate to 1 kHz and bandwith to 41 Hz
  writeByte(ACCEL_CONFIG2, c);  // Write a new ACCEL_CONFIG2 register value
  /* The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
   * but all these rates are further reduced by a factor of 5 to 200 Hz because
   * of the SMPLRT_DIV setting */

  /* ---------------> Configure Interrupts and Bypass Enable <--------------- */
  /* Set interrupt pin active high, push-pull, hold interrupt pin level HIGH
   * until interrupt cleared, clear on read of INT_STATUS, and enable
   * I2C_BYPASS_EN so additional chips can join the I2C bus and all can be
   * controlled by Linux as master */
  writeByte(INT_PIN_CFG, 0x22);  // Enable magnetometer
  writeByte(INT_ENABLE, 0X01);  // Enable data ready (bit 0) interrupt
  usleep(100*1000);

}

void Mpu9250::ReadAccelData(int16_t* destination){
  chooseDevice(MPU9250_ADDRESS);
  uint8_t rawData[6];  // x/y/z accel register data stored here
  /* Read the six raw data registers into data array */
  readBytes(ACCEL_XOUT_H, 6, &rawData[0]);
  /* Turn the MSB and LSB into a signed 16-bit value */
  destination[0] = ((int16_t)rawData[0] << 8) | rawData[1];
  destination[1] = ((int16_t)rawData[2] << 8) | rawData[3];
  destination[2] = ((int16_t)rawData[4] << 8) | rawData[5];
}

void Mpu9250::ReadGyroData(int16_t* destination){
  chooseDevice(MPU9250_ADDRESS);
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  /* Read the six raw data registers sequentially into data array */
  readBytes(GYRO_XOUT_H, 6, &rawData[0]);
  /* Turn the MSB and LSB into a signed 16-bit value */
  destination[0] = ((int16_t)rawData[0] << 8) | rawData[1];
  destination[1] = ((int16_t)rawData[2] << 8) | rawData[3];
  destination[2] = ((int16_t)rawData[4] << 8) | rawData[5];
}

void Mpu9250::ReadMagData(int16_t* destination){
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

int16_t Mpu9250::ReadTempData() {
  uint8_t rawData[2];  // temperature register data stored here
  /* Read the two raw data registers sequentially into data array */
  readBytes(TEMP_OUT_H, 2, &rawData[0]);
  /* Turn the MSB and LSB into a 16-bit value */
  return ((int16_t)rawData[0] << 8) | rawData[1];
}

void Mpu9250::GetGres() {
  /* MPU 9250 Product Specification section 3.1 */
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

void Mpu9250::GetAres() {
  /* MPU 9250 Product Specification section 3.2 */
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

void Mpu9250::GetMres() {
  /* Possible magnetometer scales (and their register bit settings) are:
   * 14 bit resolution (0) and 16 bit resolution (1) */
  switch (magnetom_scale) {
    case kMfs14Bits:
      m_res = 10.*4912./8190.; // Proper scale to return milliGauss
      break;
    case kMfs16Bits:
      m_res = 10.*4912./32760.0; // Proper scale to return milliGauss
      break;
  }
}
