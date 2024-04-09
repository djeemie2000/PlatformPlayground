#pragma once
#include <Arduino.h>
#include "fastdigitalwrite.h"

template<int Size>
class ShiftIOBank
{
public:
    ShiftIOBank() 
    : m_CsPin(9) 
    , m_OutValues(0)
    , m_PrevInValues(0) 
    , m_InValues(0) 
    {}

    void Begin(int csPin, int loadPin)
    {
        m_CsPin = csPin;
        pinMode(m_CsPin, OUTPUT);
        pinMode(loadPin, OUTPUT);
        // assumes;
        // SPI.begin();
        // SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
        m_PrevInValues = 0;
        m_InValues = 0;
    }

    void Update()
    {
        m_PrevInValues = m_InValues;
        // TODO configurable or template param

        // short low pulse on input load pin 8 = PB0
        fastDigitalWritePortB<0>(0); // digitalWrite(m_LoadPin, LOW);
        delayMicroseconds(5);
        fastDigitalWritePortB<0>(1); //digitalWrite(m_LoadPin, HIGH);
        delayMicroseconds(5);

        // hard coded CS pin 9  = PB1 
        fastDigitalWritePortB<1>(0); // digitalWrite(m_CsPin, LOW);
        m_InValues = SPI.transfer16(m_OutValues);
        fastDigitalWritePortB<1>(1); //digitalWrite(m_CsPin, HIGH);
    }

    void Set(int idx, int value)
    {
        bitWrite(m_OutValues, idx, value);
    }

    void Set(int idx)
    {
        bitSet(m_OutValues, idx);
    }

    void Clear(int idx)
    {
        bitClear(m_OutValues, idx);
    }

    int Get(int idx) const
    {
        return bitRead(m_InValues, idx);
    }

    int IsRising(int idx) const
    {
        return !bitRead(m_PrevInValues, idx) 
        && bitRead(m_InValues, idx);
    }

    int IsFalling(int idx) const
    {
        return bitRead(m_PrevInValues, idx) 
        && !bitRead(m_InValues, idx);
    }

private:
    int m_CsPin;
    uint16_t m_OutValues;
    uint16_t m_PrevInValues;
    uint16_t m_InValues;
};

template<int Size>
void TestDigitalOutBank(ShiftIOBank<Size>& bank, int repeats)
{
    Serial.print("Testing shift IO bank...");
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
void ClearAll(ShiftIOBank<Size>& bank)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        bank.Set(idx, 0);
    }
}

template<int Size>
void SetAll(ShiftIOBank<Size>& bank)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        bank.Set(idx, 1);
    }
}

template<int Size>
void PrintChanges(ShiftIOBank<Size>& bank)
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
void PrintValues(ShiftIOBank<Size>& bank)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        Serial.print(bank.Get(idx));
    }
    Serial.println();
}
