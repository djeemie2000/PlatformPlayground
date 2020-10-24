#pragma once
#include <Arduino.h>

class LedOutBank
{
public:
    static const int Capacity = 8;
    static const int Off = 0;
    static const int On = 1;
    static const int Blink = 2;

    LedOutBank(int csPin);

    void begin();
    void set(int idx, int value);
    void update(int blinkOn);

private:
    int m_CsPin;
    uint8_t m_ValuesOn;
    uint8_t m_ValuesOff;  
};

void testLedOutBank(LedOutBank &bank, int repeats = -1);
