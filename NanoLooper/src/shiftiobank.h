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
    , m_InValues(0) 
    {}

    void Begin(int csPin)
    {
        m_CsPin = csPin;
        pinMode(m_CsPin, OUTPUT);
        // assumes;
        // SPI.begin();
        // SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
    }

    void Update()
    {
        // TODO configurable or template param
        // hard coded pin 9  = PB1 
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

private:
    int m_CsPin;
    uint16_t m_OutValues;
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

