#pragma once
#include <Arduino.h>

class LedOutBank
{
public:
    static const int Size = 4;

    LedOutBank();

    void Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3);

    void LedOn(int idx);
    void LedOff(int idx);
    void LedBlink(int idx);
    void LedBlinkFast(int idx);
    void LedBlinkSlow(int idx);

    void Update(uint8_t counter);

private:
    uint8_t m_OutputPin[Size];
    uint8_t m_LedOutMask[Size];
};

void TestLedOut(LedOutBank& ledOut, int repeats);
