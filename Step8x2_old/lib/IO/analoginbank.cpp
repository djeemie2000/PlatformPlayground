#include "analoginbank.h"

AnalogInBank::AnalogInBank(int pin0, int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin7)
{
    m_Pins[0] = pin0;
    m_Pins[1] = pin1;
    m_Pins[2] = pin2;
    m_Pins[3] = pin3;
    m_Pins[4] = pin4;
    m_Pins[5] = pin5;
    m_Pins[6] = pin6;
    m_Pins[7] = pin7;
    m_UpdateIdx = -1;
}

void AnalogInBank::begin()
{
    for (int idx = 0; idx < Capacity; ++idx)
    {
        if (-1 != m_Pins[idx])
        {
            pinMode(m_Pins[idx], INPUT_PULLUP);//PULLUP?
            m_Values[idx] = -1;
        }
    }
    m_UpdateIdx = -1;
}

int AnalogInBank::get(int idx) const
{
    if (0 <= idx && idx < Capacity)
    {
        return m_Values[idx];
    }
    return -1;
}

void AnalogInBank::update()
{
    for (int idx = 0; idx < Capacity; ++idx)
    {
        if (-1 != m_Pins[idx])
        {
            m_Values[idx] = analogRead(m_Pins[idx]);
        }
    }
}

void AnalogInBank::updateOne()
{
    if(m_UpdateIdx<0)
    {
        update();
        m_UpdateIdx = 0;
    }
    else
    {
        if (-1 != m_Pins[m_UpdateIdx])
        {
            m_Values[m_UpdateIdx] = analogRead(m_Pins[m_UpdateIdx]);
        }
        ++m_UpdateIdx;
        if(Capacity<=m_UpdateIdx)
        {
            m_UpdateIdx = 0;
        }
    }
}


// void testAnalogInBank(AnalogInBank &bank, int repeats)
// {
//     for (int repeat = 0; repeat < repeats || repeats < 0; ++repeat)
//     {
//         for (int idx = 0; idx < AnalogInBank::Capacity; ++idx)
//         {
//             bank.set(idx, 1);
//             bank.update();
//             delay(200);
//             bank.set(idx, 0);
//             bank.update();
//             delay(200);
//             bank.set(idx, 1);
//         }
//     }
// }
