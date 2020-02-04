#pragma once 

#include <mbed.h>
#include "DigitalInBank.h"

class TTP8229InBank : public DigitalInBank
{
public:
    TTP8229InBank(I2C* i2c);

    int Size() const;
    int Get(int idx) const;
    void Read();

private:
    I2C* m_i2c;
    uint16_t m_TouchState;// bit = 1 => touched, bit is 0 => not touched
};
