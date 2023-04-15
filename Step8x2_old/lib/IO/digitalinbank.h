#pragma once
#include <Arduino.h>

class DigitalInBank
{
public:
    static const int Capacity = 8;

    DigitalInBank(int pin0 = -1, int pin1 = -1, int pin2 = -1, int pin3 = -1, int pin4 = -1, int pin5 = -1, int pin6 = -1, int pin7 = -1);

    void begin();
    int get(int idx) const;
    void update();

private:
    int m_Pins[Capacity];
    int m_Values[Capacity];
};

//void testDigitalInBank(DigitalInBank &bank, int repeats = -1);
