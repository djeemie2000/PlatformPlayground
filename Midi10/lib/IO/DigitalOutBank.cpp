#include "DigitalOutBank.h"

DigitalOutBank::DigitalOutBank()
{
    for (int idx = 0; idx < Capacity; ++idx)
    {
        m_Pins[idx] = -1;
        m_ValuesOn[idx] = 0;
        m_ValuesOff[idx] = 0;
    }
}

void DigitalOutBank::begin(int pin0, int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin7)
{
    m_Pins[0] = pin0;
    m_Pins[1] = pin1;
    m_Pins[2] = pin2;
    m_Pins[3] = pin3;
    m_Pins[4] = pin4;
    m_Pins[5] = pin5;
    m_Pins[6] = pin6;
    m_Pins[7] = pin7;

    for (int idx = 0; idx < Capacity; ++idx)
    {
        if (-1 != m_Pins[idx])
        {
            pinMode(m_Pins[idx], OUTPUT);
            m_ValuesOn[idx] = 0;
            m_ValuesOff[idx] = 0;
        }
    }
}

void DigitalOutBank::set(int idx, int value)
{
    if (0 <= idx && idx < Capacity)
    {
        if (value == On)
        {
            m_ValuesOn[idx] = 1;
            m_ValuesOff[idx] = 1;
        }
        else if (value == Off)
        {
            m_ValuesOn[idx] = 0;
            m_ValuesOff[idx] = 0;
        }
        else
        {
            m_ValuesOn[idx] = 1;
            m_ValuesOff[idx] = 0;
        }
    }
}

void DigitalOutBank::update(int blinkOn)
{
    for (int idx = 0; idx < Capacity; ++idx)
    {
        if (-1 != m_Pins[idx])
        {
            digitalWrite(m_Pins[idx], blinkOn ? m_ValuesOn[idx] : m_ValuesOff[idx]);
        }
    }
}

void testDigitalOutBank(DigitalOutBank &bank, int repeats)
{
    for (int repeat = 0; repeat < repeats || repeats < 0; ++repeat)
    {
        for (int idx = 0; idx < DigitalOutBank::Capacity; ++idx)
        {
            bank.set(idx, 1);
            bank.update(1);
            delay(200);
            bank.set(idx, 0);
            bank.update(1);
            delay(200);
            bank.set(idx, 1);
        }
    }
}
