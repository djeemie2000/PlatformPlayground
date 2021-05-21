#pragma once
#include <Arduino.h>

class DigitalOutBank
{
public:
    static const int Capacity = 10;

    DigitalOutBank();

    void begin(int pin0 = -1, int pin1 = -1, int pin2 = -1, int pin3 = -1, int pin4 = -1, int pin5 = -1, int pin6 = -1, int pin7 = -1, int pin8 = -1, int pin9 = -1);
    void set(int idx, uint8_t value);
    void update();

private:
    uint8_t m_Pins[Capacity];
    uint8_t m_Values[Capacity];
};

void testDigitalOutBank(DigitalOutBank &bank, int repeats = -1);
