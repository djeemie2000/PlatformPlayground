#pragma once

#include <mbed.h>
#include "DigitalInBank.h"
#include "mpr121.h"
#include "GateIn.h"

class Mpr121InBank : public DigitalInBank
{
public:
    Mpr121InBank(I2C* i2c, PinName irqPin);

    int Size() const;
    int Get(int idx) const;
    void Read();

private:
    Mpr121 m_Touchpad;
    DigitalIn m_In;
    GateState m_State;
    int m_TouchState;
};
