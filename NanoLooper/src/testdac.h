#pragma once
#include <Arduino.h>

template<class DacType>
void TestDacSlow(DacType& dac, int increase, int delayMs)
{
    int value = 0;
    while(true)
    {
        dac.Write(value);
        dac.WriteR(value);
        value += increase;
        if(256>= value)
        {
            value -= 256;
        }
        delay(delayMs);
    }
}

template<class DacType>
void TestDacFastAscending(DacType& dac, int delayUsec)
{
    uint8_t value = 0x00;
    while(true)
    {
        dac.Write(value);
        dac.WriteR(value);
        ++value;
        delayMicroseconds(delayUsec);
    }
}

template<class DacType>
void TestDacFastDescending(DacType& dac, int delayUsec)
{
    uint8_t value = 0xFF;
    while(true)
    {
        dac.Write(value);
        dac.WriteR(value);
        --value;
        delayMicroseconds(delayUsec);
    }
}
