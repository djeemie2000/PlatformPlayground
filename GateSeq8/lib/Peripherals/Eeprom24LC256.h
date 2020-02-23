#pragma once

#include <Arduino.h>
#include "MemoryBank.h"

class Eeprom24LC256 : public MemoryBank
{
public:
    enum Address{
        AllLow = 0x50,
        A0High =  0x52,
        A1High =  0x54,
        A2High =  0x58,
        A0A1High =  0x56,
        A0A2High =  0x5A,
        A1A2High =  0x5C,        
        AllHigh = 0x5E
    };
    Eeprom24LC256(Address address);
    //TODO prevent write for approx 5 milliseconds after a write

    int BankSize() const    {   return 64;      }
    int NumBanks() const    {   return 512;    }
    int WriteBank(int bank, const uint8_t* data, int size, int offset);//returns actual # written
    int ReadBank(int bank, uint8_t* data, int size, int offset);//returns actual # read
    //TODO offset support in bank

private:
    void WriteEepromAdress(int eepromAddress);
    void ack_pooling();
    int WriteBlock(int eepromAddress, const uint8_t* data, int size);
    int ReadBlock(int eepromAddress, uint8_t* data, int size);


    //I2C* m_I2c;
    int m_Address;
};
