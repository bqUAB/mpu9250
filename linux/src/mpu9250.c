#include <mpu9250.h>

//==============================================================================
//= Set of useful function to access acceleration. gyroscope, magnetometer,    =
//= and temperature data                                                       =
//==============================================================================

uint8_t readByte(uint8_t address, uint8_t subAddress){
  uint8_t data; // `data` will store the register data
  return data;
}

// uint8_t MPU9250::readByte(uint8_t address, uint8_t subAddress)
// {
//   uint8_t data; // `data` will store the register data
//   Wire.beginTransmission(address);         // Initialize the Tx buffer
//   Wire.write(subAddress);                  // Put slave register address in Tx buffer
//   Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
//   Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address
//   data = Wire.read();                      // Fill Rx buffer with result
//   return data;                             // Return data read from slave register
// }
