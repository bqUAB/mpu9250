// Simplified I2C class based on:
// http://docs.micropython.org/en/latest/wipy/library/machine.I2C.html

#ifndef I2C_H_
#define I2C_H_

#include <stdio.h>  // Needed for printf, snprintf, perror
#include <fcntl.h>  // Needed for open()
#include <unistd.h>  // Needed for write, close
#include <stdlib.h>  // Needed for exit()
#include <cstdint>  // Needed for uint8_t
#include <sys/ioctl.h>  // Needed for ioctl
#include <linux/i2c-dev.h>  // Needed to use the I2C Linux driver (I2C_SLAVE)


class I2cBus {
  private:
    int file_ = 0;

    bool SetSlaveAddr_(uint16_t addr);

  public:
    I2cBus(uint bus_n);

    // ----------------------- Standard bus operations -----------------------
    // The following methods implement the standard I2C master read and write
    // operations that target a given slave device.
    bool ReadFromInto(uint16_t addr, uint8_t* buff);
    // int WriteTo(int addr, uint8_t* buff);

    // -------------------------- Memory Operations --------------------------
    // Some I2C devices act as a memory device (or set of registers) that can
    // be read from and written to. In this case there are two addresses
    // associated with an I2C transaction: the slave address and the memory
    // address. The following methods are convenience functions to communicate
    // with such devices.

    bool WriteToMem(uint16_t addr, uint8_t mem_addr, uint8_t data);
    bool WriteToMemFrom(uint16_t addr, uint8_t mem_addr, uint n_bytes,
                       uint8_t* buff_ptr);
    bool ReadFromMem(uint16_t addr, uint8_t mem_addr, uint8_t* data_ptr);
    bool ReadFromMemInto(uint16_t addr, uint8_t mem_addr, uint n_bytes,
                        uint8_t* buff_ptr);

};  // Class I2C

#endif // I2C_H_
