#pragma once
#include <Arduino.h>

#include "fastdigitalpintooffset.h"

template<int Size>
class FastDigitalInBank
{
public:
    FastDigitalInBank(){}

    void Begin()
    {
        m_InputMask = 0x00000000;
        m_PrevInputMask = 0x00000000;
        for(int idx = 0; idx<Size; ++idx)
        {
            m_Offset[idx] = -1;
        }
    }

    void Assign(int idx, int pin, bool pullup)
    {
        int offset = PinToOffset(pin);
        if(-1 != offset)
        {
            m_Offset[idx] = offset;
            pinMode(pin, pullup?INPUT_PULLUP:INPUT);
        }
    }

    int Get(int idx) const
    {
        if(-1 != m_Offset[idx])
        {
            return bitRead(m_InputMask, m_Offset[idx]);
        }
        return 0;
    }

    int IsRising(int idx) const
    {
        if(-1 != m_Offset[idx])
        {
            return !bitRead(m_PrevInputMask, m_Offset[idx]) 
            && bitRead(m_InputMask, m_Offset[idx]);
        }
        return 0;
    }

    int IsFalling(int idx) const
    {
        if(-1 != m_Offset[idx])
        {
            return bitRead(m_PrevInputMask, m_Offset[idx]) 
            && !bitRead(m_InputMask, m_Offset[idx]);
        }
        return 0;
    }

    void Update()
    {
        m_PrevInputMask = m_InputMask;
        m_InputMask = uint32_t(PIND)<<16 | uint32_t(PINC)<<8 | uint32_t(PINB);
    }

private:
    uint32_t m_InputMask;
    uint32_t m_PrevInputMask;
    int m_Offset[Size];
};

template<int Size>
void PrintChanges(FastDigitalInBank<Size>& bank)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        if(bank.IsFalling(idx))
        {
            Serial.print("input ");
            Serial.print(idx);
            Serial.println(" is falling");
        }
        else if(bank.IsRising(idx))
        {
            Serial.print("input ");
            Serial.print(idx);
            Serial.println(" is rising");
        }
    }
}

template<int Size>
void PrintValues(FastDigitalInBank<Size>& bank)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        Serial.print(bank.Get(idx));
    }
    Serial.println();
}

