#pragma once
#include <Arduino.h>

#include "fastdigitalpintooffset.h"

template<int Size>
class FastDigitalOutBank
{
public:
    FastDigitalOutBank() 
    : m_IgnoreMask(0xFFFFFFFF)
    , m_OutputMask(0)
    {}

    void Begin()
    {
        m_IgnoreMask = 0xFFFFFFFF;
        m_OutputMask = 0x00000000;
        for(int idx = 0; idx<Size; ++idx)
        {
            m_Offset[idx] = -1;
        }
    }

    void Assign(int idx, int pin)
    {
        int offset = PinToOffset(pin);
        if(-1 != offset)
        {
            m_Offset[idx] = offset;
            bitClear(m_IgnoreMask, offset);
            bitClear(m_OutputMask, offset);            
            pinMode(pin, OUTPUT);
        }
    }

    void Set(int idx)
    {
        if(-1 != m_Offset[idx])
        {
            bitSet(m_OutputMask, m_Offset[idx]);
        }
    }

    void Clear(int idx)
    {
        if(-1 != m_Offset[idx])
        {
            bitClear(m_OutputMask, m_Offset[idx]);
        }
    }

    void Set(int idx, int value)
    {
        if(-1 != m_Offset[idx])
        {
            bitWrite(m_OutputMask, m_Offset[idx], value);
        }
    }

    void ClearAll()
    {
        m_OutputMask = 0x00000000;
    }

    void Update()
    {
        uint8_t ignoreMaskB = m_IgnoreMask & 0xFF;
        uint8_t outputMaskB = m_OutputMask & 0xFF;

        uint8_t ignoreMaskC = m_IgnoreMask>>8 & 0xFF;
        uint8_t outputMaskC = m_OutputMask>>8 & 0xFF;

        uint8_t ignoreMaskD = m_IgnoreMask>>16 & 0xFF;
        uint8_t outputMaskD = m_OutputMask>>16 & 0xFF;

        PORTB = (PORTB & ignoreMaskB) | outputMaskB; 
        PORTC = (PORTC & ignoreMaskC) | outputMaskC; 
        PORTD = (PORTD & ignoreMaskD) | outputMaskD; 
    }


private:
    uint32_t m_IgnoreMask;
    uint32_t m_OutputMask;
    int m_Offset[Size];
};

// template<int Size>
// void SetOutput(FastDigitalOutBank<Size>& bank,int idx, int value)
// {
//     if(!value)
//     {
//         bank.Clear(idx);
//     }
//     else
//     {
//         bank.Set(idx);
//     }
// }

template<int Size>
void TestDigitalOutBank(FastDigitalOutBank<Size>& bank, int repeats)
{
    Serial.print("Testing fast digital out bank...");
    const int delayMSecs = 300;
    for(int repeat = 0; repeat<repeats;++repeat)
    {
        for(int idx = 0; idx<Size;++idx)
        {
            bank.Set(idx);
            bank.Update();
            delay(delayMSecs);
            bank.Clear(idx);
            bank.Update();
        }
    }
    Serial.println(" done");
}

template<int Size>
void AllClear(FastDigitalOutBank<Size>& bank)
{
    bank.ClearAll();
    bank.Update();
}
