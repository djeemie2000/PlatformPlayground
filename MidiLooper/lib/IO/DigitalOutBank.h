#pragma once
#include <Arduino.h>

class DigitalOutBank
{
public:
    static const int Capacity = 8;

    DigitalOutBank(int pin0 = NC, int pin1 = NC, int pin2 = NC, int pin3 = NC, int pin4 = NC, int pin5 = NC, int pin6 = NC, int pin7 = NC);

    void begin();
    void set(int idx, int value);
    void update();

private:
    int m_Pins[Capacity];
    int m_Values[Capacity];
};

void testDigitalOutBank(DigitalOutBank &bank, int repeats = -1);
