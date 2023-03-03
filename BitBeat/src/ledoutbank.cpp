#include "ledoutbank.h"

LedOutBank::LedOutBank()
{
    for (int idx = 0; idx < LedOutBank::Size; ++idx)
    {
        m_LedOutMask[idx] = 0x00;
    }
}

void LedOutBank::Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3)
{
    m_OutputPin[0] = pin0;
    m_OutputPin[1] = pin1;
    m_OutputPin[2] = pin2;
    m_OutputPin[3] = pin3;

    for (int idx = 0; idx < LedOutBank::Size; ++idx)
    {
        m_LedOutMask[idx] = 0x00;
        pinMode(m_OutputPin[idx], OUTPUT);
        digitalWrite(m_OutputPin[idx], LOW);
    }
}

void LedOutBank::LedOn(int idx)
{
    m_LedOutMask[idx] = 0xFF;
}

void LedOutBank::LedOff(int idx)
{
    m_LedOutMask[idx] = 0x00;
}

void LedOutBank::LedBlink(int idx)
{
    m_LedOutMask[idx] = 0x40;
}

void LedOutBank::LedBlinkFast(int idx)
{
    m_LedOutMask[idx] = 0x10;
}

void LedOutBank::LedBlinkSlow(int idx)
{
    m_LedOutMask[idx] = 0x80;
}

void LedOutBank::Update(uint8_t counter)
{
    for (int idx = 0; idx < LedOutBank::Size; ++idx)
    {
        digitalWrite(m_OutputPin[idx], m_LedOutMask[idx] == 0xFF ? 1 : (m_LedOutMask[idx] & counter));
    }
}

void ApplyLedOut(LedOutBank &ledOut)
{
    for (int idx = 0; idx < 2000; ++idx)
    {
        uint8_t counter = millis() >> 2;
        ledOut.Update(counter);
        delay(1);
    }
}

void TestLedOut(LedOutBank &ledOut, int repeats)
{
    Serial.print("Testing led out bank...");
    for (int repeat = 0; repeat < repeats; ++repeat)
    {
        Serial.println("Led On");
        for (int idx = 0; idx < LedOutBank::Size; ++idx)
        {
            ledOut.LedOn(idx);
        }
        ApplyLedOut(ledOut);

        Serial.println("Led Off");
        for (int idx = 0; idx < LedOutBank::Size; ++idx)
        {
            ledOut.LedOff(idx);
        }
        ApplyLedOut(ledOut);

        Serial.println("Led Blink");
        for (int idx = 0; idx < LedOutBank::Size; ++idx)
        {

            ledOut.LedBlink(idx);
        }
        ApplyLedOut(ledOut);

        Serial.println("Led Blink Fast");
        for (int idx = 0; idx < LedOutBank::Size; ++idx)
        {
            ledOut.LedBlinkFast(idx);
        }
        ApplyLedOut(ledOut);

        Serial.println("Led Blink Slow");
        for (int idx = 0; idx < LedOutBank::Size; ++idx)
        {
            ledOut.LedBlinkSlow(idx);
        }
        ApplyLedOut(ledOut);
    }
}
