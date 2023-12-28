#pragma once
#include <Arduino.h>

template<int Size>
class FastDigitalOutBank
{
public:
    //static const int Size = 12;//TODO template parameter

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

    void Update()
    {
        uint8_t ignoreMaskB = m_IgnoreMask & 0xFF;
        uint8_t outputMaskB = m_OutputMask & 0xFF;
        PORTB = (PORTB & ignoreMaskB) | outputMaskB; 

        uint8_t ignoreMaskC = m_IgnoreMask>>8 & 0xFF;
        uint8_t outputMaskC = m_OutputMask>>8 & 0xFF;
        PORTC = (PORTC & ignoreMaskC) | outputMaskC; 


        uint8_t ignoreMaskD = m_IgnoreMask>>16 & 0xFF;
        uint8_t outputMaskD = m_OutputMask>>16 & 0xFF;
        PORTD = (PORTD & ignoreMaskD) | outputMaskD; 
    }


private:
    int PinToOffset(int pin)
    {
        // offset order is port B (0..7) port C (8..15) port D (16..23)
        if(pin >= 2 && pin<=7)
        {
            //PD2 .. PD7
            return 16+pin;
        }
        if(pin >= 8 && pin<= 13)
        {
            // PB0 .. PB5
            return pin-8;// 0 + pin - 8
        }
        if(pin>=14 && pin <= 19)
        {
            // PC0 .. PC5
            return pin-6; // 8 + pin - 14
        }
        return -1;//TODO
    }

    uint32_t m_IgnoreMask;
    uint32_t m_OutputMask;
    int m_Offset[Size];
};

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
    for(int idx = 0; idx<Size;++idx)
    {
        bank.Clear(idx);
    }
    bank.Update();
}
