#include "i2c.h"


// I2C bus constructor
I2cBus::I2cBus(uint bus_n) {
  // Access an I2C bus adapter from a C++ program

  char file_name[15]; // To hold /dev/i2c-#
  snprintf(file_name, sizeof(file_name), "/dev/i2c-%d", bus_n);

  // Open I2C bus driver
  if ((file_ = open(file_name, O_RDWR)) < 0) {
    // ERROR HANDLING: you can check errno to see what went wrong
    perror("Failed to open the I2C bus.\n");
    exit(1);
  }
}

bool I2cBus::SetSlaveAddr_(uint16_t addr) {
  bool success = false;

  // Input output control setup to the slave device.
  if (ioctl(file_, I2C_SLAVE, addr) < 0) {
    success = false;
    perror("Failed to acquire bus access and/or talk to slave.\n");
    // ERROR HANDLING; you can check errno to see what went wrong
    exit(1);
  } else {
    success = true;
  }

  return success;
}

bool I2cBus::ReadFromInto(uint16_t addr, uint8_t* buff_ptr) {
  // Read into buff from the slave specified by addr. The number of bytes read
  // will be the length of buff

  bool success = false;

  SetSlaveAddr_(addr);
  if (read(file_, buff_ptr, sizeof(buff_ptr)) == sizeof(buff_ptr)) {
    success = true;
  } else {
    success = false;
    perror("I2C read from slave into buffer failed.\n");
    // ERROR HANDLING; you can check errno to see what went wrong
    exit(1);
  }

  return success;
}

bool I2cBus::WriteToMem(uint16_t addr, uint8_t mem_addr, uint8_t data){
  // Write data to the slave specified by addr starting from the memory
  // address specified by mem_addr.

  bool success = false;
  uint8_t w_buf[2];
  w_buf[0] = mem_addr;
  w_buf[1] = data;

  SetSlaveAddr_(addr);

  // Write to defined register
  if (write(file_, &w_buf, sizeof(w_buf)) == sizeof(w_buf)){
    success = true;
  } else {
    success = false;
    perror("I2C write to memory failed.\n");
    // ERROR HANDLING; you can check errno to see what went wrong
    exit(1);
  }

  return success;
}

bool I2cBus::WriteToMemFrom(uint16_t addr, uint8_t mem_addr, uint n_bytes,
                            uint8_t* buff_ptr) {
  // Write buff to the slave specified by addr starting from the memory
  // address specified by mem_addr.

  bool success = false;
  uint8_t w_buff[1 + n_bytes];  // cannot create type information for type
                                // 'int [(n_bytes + 1)]' because it involves
                                // types of variable size

  SetSlaveAddr_(addr);
  w_buff[0] = mem_addr;
  // Shift and then fill the buffer
  for (uint i = 1; i <= n_bytes; i++) {
    w_buff[i] = buff_ptr[i-1];
  }

  // Write to defined register
  if (write(file_, &w_buff, sizeof(w_buff)) == int(sizeof(w_buff))) {
                                            // ^ int casting due to
                                            // uint comparison warning
    success = true;
  } else {
    success = false;
    perror("I2C write to memory from buffer failed.\n");
    // ERROR HANDLING; you can check errno to see what went wrong
    exit(1);
  }

  return success;
}

bool I2cBus::ReadFromMem(uint16_t addr, uint8_t mem_addr, uint8_t* data_ptr) {
  // Read a byte from the slave specified by addr from the memory address
  // specified by mem_addr.

  bool success = false;

  SetSlaveAddr_(addr);

  // Write to defined register
  if (write(file_, &mem_addr, sizeof(mem_addr)) == sizeof(mem_addr)) {
    // Read back value
    if (read(file_, data_ptr, sizeof(uint8_t)) == sizeof(uint8_t)) {
      success = true;
    } else {
      success = false;
      perror("I2C read from memory failed.\n");
      // ERROR HANDLING; you can check errno to see what went wrong
      exit(1);
    }
  }

  return success;
}

bool I2cBus::ReadFromMemInto(uint16_t addr, uint8_t mem_addr, uint n_bytes,
                             uint8_t* buff_ptr)
{
  // Read n_bytes into buff from the slave specified by addr starting from
  // the memory address specified by mem_addr.

  bool success = false;

  SetSlaveAddr_(addr);
  // Write to defined register
  if (write(file_, &mem_addr, sizeof(mem_addr)) == sizeof(mem_addr)) {
    // read back value
    if (read(file_, buff_ptr, n_bytes) == int(n_bytes)) {
      success = true;
    } else {
      success = false;
      perror("I2C read from memory into buffer failed.\n");
      // ERROR HANDLING; you can check errno to see what went wrong
      exit(1);
    }
  }

  return success;
}
