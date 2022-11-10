#include "ledout.h"

LedOut::LedOut() 
: m_LedOutMask(0x00)
{}

void LedOut::Begin()
{
    m_LedOutMask = 0x00;
}

void LedOut::LedOn()
{
//    Serial.println("LedOn");
    m_LedOutMask = 0xFF;
}
    
void LedOut::LedOff()
{
    //Serial.println("LedOff");
    m_LedOutMask = 0x00;
}

void LedOut::LedBlink()
{
    //Serial.println("LedBlink");
    m_LedOutMask = 0x40;
}

void LedOut::LedBlinkFast()
{
    m_LedOutMask = 0x10;
}

void LedOut::LedBlinkSlow()
{
    m_LedOutMask = 0x80;
}

void LedOut::Apply(uint8_t counter, uint8_t ledOutPin)
{
    //DigitalOut class?
    digitalWrite(ledOutPin, m_LedOutMask == 0xFF ? 1 : (m_LedOutMask & counter));
}

void ApplyLedOut(LedOut& ledOut, uint8_t ledOutPin)
{
    for(int idx = 0; idx<1000; ++idx)
    {
        uint8_t counter = millis() >> 2;
        ledOut.Apply(counter, ledOutPin);
        delay(1);
    }
}

void TestLedOut(LedOut& ledOut, uint8_t ledOutPin, int repeats)
{
//    const int delayMSec = 300;
    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        Serial.println("Led On");
        ledOut.LedOn();
        ApplyLedOut(ledOut, ledOutPin);
        
        Serial.println("Led Off");
        ledOut.LedOff();
        ApplyLedOut(ledOut, ledOutPin);

        Serial.println("Led Blink");
        ledOut.LedBlink();
        ApplyLedOut(ledOut, ledOutPin);

        Serial.println("Led Blink Fast");
        ledOut.LedBlinkFast();
        ApplyLedOut(ledOut, ledOutPin);

        Serial.println("Led Blink Slow");
        ledOut.LedBlinkSlow();
        ApplyLedOut(ledOut, ledOutPin);
    }
}

