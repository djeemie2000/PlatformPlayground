#pragma once

#include <mbed.h>
#include "DigitalInBank.h"
#include "mpr121.h"

class Mpr121InBank : public DigitalInBank
{
public:
    Mpr121InBank(PinName sda, PinName sdc);

    int Size() const;
    int Get(int idx) const;
    void Read();

private:
    I2C m_I2C;
    Mpr121 m_Touchpad;
    int m_TouchState;
};
