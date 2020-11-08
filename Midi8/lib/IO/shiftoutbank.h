#pragma once
#include <Arduino.h>

class ShiftOutBank
{
public:
    static const int Capacity = 8;

    ShiftOutBank(int csPin);

    void begin();
    void set(int idx, int value);
    void update();

private:
    int m_CsPin;
    uint8_t m_Values;
};

void testDigitalOutBank(ShiftOutBank &bank, int repeats = -1);