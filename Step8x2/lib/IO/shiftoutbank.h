#pragma once
#include <Arduino.h>

class ShiftOutBank
{
public:
    static const int Capacity = 16;

    ShiftOutBank(int csPin);

    void begin();
    void set(int idx, int value);
    int get(int idx);
    void update();

private:
    int m_CsPin;
    uint16_t m_Values;
    uint16_t m_ValuesIn;
};

void testDigitalOutBank(ShiftOutBank &bank, int repeats = -1);
