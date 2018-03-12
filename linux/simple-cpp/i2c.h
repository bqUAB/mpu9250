/* Simplified I2C class based on:
http://docs.micropython.org/en/latest/wipy/library/machine.I2C.html
*/
#ifndef I2C_H
#define I2C_H

#include <stdio.h>  // Needed for printf, snprintf, perror
#include <cstdint>  // Needed for unit uint8_t data type
#include <fcntl.h>  // Needed for open()
#include <unistd.h>  // Needed for write, close
#include <stdlib.h>  // Needed for exit()
#include <sys/ioctl.h>  // Needed for ioctl
// #include <linux/i2c-dev.h>  // Needed to use the I2C Linux driver (I2C_SLAVE)

class I2cBus {
  private:
    int file_ = 0;
    int list_[5];

    void SetSlaveAddr_(uint8_t addr);

  public:
    I2cBus(int bus_n);

    /* -------------------------- Memory Operations --------------------------
    Some I2C devices act as a memory device (or set of registers) that can be
    read from and written to. In this case there are two addresses associated
    with an I2C transaction: the slave address and the memory address. The
    following methods are convenience functions to communicate with such
    devices.*/

    void WriteToMem(uint8_t addr, uint8_t mem_addr, uint8_t data);
    uint8_t ReadFromMem(uint8_t addr, uint8_t mem_addr);
    void ReadFromMemInto(uint8_t addr, uint8_t mem_addr, int n_bytes,
                         uint8_t* data_buff);

};

#endif // I2C_H
