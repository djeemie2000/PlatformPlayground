#pragma once

#include <mbed.h>
#include "MemoryBank.h"

class Eeprom24LC256 : public MemoryBank
{
public:
    enum Address{
        AllLow = 0xA0,
        A0High =  0xA2,
        A1High =  0xA4,
        A2High =  0xA8,
        A0A1High =  0xA6,
        A0A2High =  0xAA,
        A1A2High =  0xAC,        
        AllHigh = 0xAE
    };
    Eeprom24LC256(I2C* i2c, Address address);
    //TODO prevent write for approx 5 milliseconds after a write

    int BankSize() const    {   return 64;      }
    int NumBanks() const    {   return 4096;    }
    int WriteBank(int bank, const uint8_t* data, int size);//returns actual # written
    int ReadBank(int bank, uint8_t* data, int size);//returns actual # read
    //TODO offset support in bank

private:
    void WriteAdress(int eepromAddress);

    I2C* m_I2c;
    int m_Address;
};
