#pragma once
#include <Arduino.h>

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

    void Apply(uint8_t counter, uint8_t ledOutPin);//DigitalOut class?

private:
    uint8_t m_LedOutMask;
};

void TestLedOut(LedOut& ledOut, uint8_t ledOutPin, int repeats);
