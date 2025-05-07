#pragma once
#include <Arduino.h>
#include "fastdigitaloutbank.h"

// template<int Size>
// class FastDigitalOutBank;

class LedOut
{
public:
    LedOut();

    void Begin();

    void LedOn();
    void LedOff();
    void LedBlink();
    void LedBlinkFast();
    void LedBlinkSlow();

    template<int Size>
    void Apply(uint8_t counter, int idx, FastDigitalOutBank<Size>& bank);

private:
    uint8_t m_LedOutMask;
};

template<int Size>
void LedOut::Apply(uint8_t counter, int idx, FastDigitalOutBank<Size>& bank)
{
    //led off on other indices
    for(int index=0; index<Size; ++index)
    {
        if(index==idx)
        {
            if(m_LedOutMask == 0xFF || (m_LedOutMask & counter))
            {
                bank.Set(idx);
            }
            else
            {
                bank.Clear(idx);
            }
        }
        else
        {
            bank.Clear(idx);
        }
    }
}

template<int Size>
void ApplyLedOut(LedOut& ledOut, int idx, FastDigitalOutBank<Size>& bank)
{
    for(int idx = 0; idx<1000; ++idx)
    {
        uint8_t counter = millis() >> 2;
        ledOut.Apply(counter, idx, bank);
        delay(1);
    }
}

template<int Size>
void TestLedOut(LedOut& ledOut, int idx, FastDigitalOutBank<Size>& bank, int repeats)
{
    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        Serial.println("Led On");
        ledOut.LedOn();
        ApplyLedOut(ledOut, idx, bank);
        
        Serial.println("Led Off");
        ledOut.LedOff();
        ApplyLedOut(ledOut, idx, bank);

        Serial.println("Led Blink");
        ledOut.LedBlink();
        ApplyLedOut(ledOut, idx, bank);

        Serial.println("Led Blink Fast");
        ledOut.LedBlinkFast();
        ApplyLedOut(ledOut, idx, bank);

        Serial.println("Led Blink Slow");
        ledOut.LedBlinkSlow();
        ApplyLedOut(ledOut, idx, bank);
    }

}
