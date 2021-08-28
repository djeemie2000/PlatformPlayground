#include "TouchInBank.h"

TouchInBank::TouchInBank(int pin0, int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin7)
{
    m_Pins[0] = pin0;
    m_Pins[1] = pin1;
    m_Pins[2] = pin2;
    m_Pins[3] = pin3;
    m_Pins[4] = pin4;
    m_Pins[5] = pin5;
    m_Pins[6] = pin6;
    m_Pins[7] = pin7;
}

void TouchInBank::begin(int lowThreshold, int highThreshold)
{
    m_LowThreshold = lowThreshold;
    m_HighThreshold = highThreshold;

    for (int idx = 0; idx < Capacity; ++idx)
    {
        m_Values[idx] = -1;
        m_State[idx] = 0;
        // if (-1 != m_Pins[idx])
        // {
        //     pinMode(m_Pins[idx], INPUT_PULLUP);//PULLUP?
        //     m_Values[idx] = -1;
        // }
    }
}

int TouchInBank::get(int idx) const
{
    if (0 <= idx && idx < Capacity)
    {
        return m_State[idx];
    }
    return -1;
}

void TouchInBank::update()
{
    for (int idx = 0; idx < Capacity; ++idx)
    {
        if (-1 != m_Pins[idx])
        {
            // smoothing + hysteresis
            if(-1 == m_Values[idx])
            {
                // first
                m_Values[idx] = touchRead(m_Pins[idx]);
            }
            else
            {
                const int smoothN = 3;// (7*m_value + value)/8
                m_Values[idx] = ( (m_Values[idx]<<smoothN) - m_Values[idx] + touchRead(m_Pins[idx])) >> smoothN;
            }

            if(m_State[idx] && m_HighThreshold<m_Values[idx])
            {
                m_State[idx] = 0;
            }
            else if(!m_State[idx] && m_Values[idx]<m_LowThreshold)
            {
                m_State[idx] = 1;
            }
        }
    }
}

int TouchInBank::getValue(int idx)
{
    if (0 <= idx && idx < Capacity)
    {
        return m_Values[idx];
    }
    return -1;
}


// void testTouchInBank(TouchInBank &bank, int repeats)
// {
//     for (int repeat = 0; repeat < repeats || repeats < 0; ++repeat)
//     {
//         for (int idx = 0; idx < TouchInBank::Capacity; ++idx)
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

void PrintTouchInBank(TouchInBank &touchPad, HardwareSerial &debugSerial)
{
    debugSerial.print("touchPad ");
    for (int pad = 0; pad < 8; ++pad)
    {
        debugSerial.print(touchPad.get(pad));
    }
    debugSerial.print("   ");
    for (int pad = 0; pad < 8; ++pad)
    {
        debugSerial.print(touchPad.getValue(pad));
        debugSerial.print(" ");
    }
    debugSerial.println();
}

void TestTouchInBank(TouchInBank &touchPad, HardwareSerial &debugSerial, int count)
{
    int cntr = 0;
    while (cntr < count || count == -1)
    {
        for(int repeat = 0; repeat<100; ++repeat)
        {
            touchPad.update();
            delay(5);
        }
        PrintTouchInBank(touchPad, debugSerial);
        ++cntr;
    }
}
