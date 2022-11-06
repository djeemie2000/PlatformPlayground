#pragma once
#include <Arduino.h>

class LedOut
{
public:
    LedOut();

    void Begin(uint8_t ledOutPin);

    void LedOn();
    void LedOff();
    void LedBlink();
    void LedBlinkFast();
    void LedBlinkSlow();

    void Update(uint8_t counter);

private:
    uint8_t m_LedOutPin;
    uint8_t m_LedOutMask;
};
