#include "ScanI2C.h"

void ScanI2C(I2C& i2c, Serial& serial)
{
  serial.printf("I2C scanning...\r\n");
  for(int address = 1; address < 127; address++ ) 
  {
    i2c.start();
    int error = i2c.write(address << 1); //We shift it left because mbed takes in 8 bit addreses
    i2c.stop();
    if (error == 1)
    {
      serial.printf("I2C device found at address 0x%X\r\n", address); //Returns 7-bit addres
    }
  }
  serial.printf("I2C scan done\r\n");
}
