/*******************************************************************************
* Demo application using the MPU 9250
* by Byron Quezada
*
* Example of how to use the MPU 9250 throuh I2C using Linux by using an ARM
* processor running Debian. This demonstration shows how to write and read the
* MPU 9250 registers by accessing them through the built-in I2C kernel driver.
* The I2C adapter is selected from userspace through the /dev interface. The
* i2c-dev module is needed but it's loaded by default.
*
* Created 10 November 2017 based on:
* <http://elinux.org/Interfacing_with_I2C_Devices>
* <https://www.kernel.org/doc/Documentation/i2c/dev-interface>
*******************************************************************************/

int main(){
  printf("===== MPU 9250 Test using Linux =====\n");
  return 0;
}
