#pragma once
#include <Arduino.h>

class AnalogInBank
{
public:
    static const int Capacity = 8;

    AnalogInBank(int pin0 = -1, int pin1 = -1, int pin2 = -1, int pin3 = -1, int pin4 = -1, int pin5 = -1, int pin6 = -1, int pin7 = -1);

    void begin();
    int get(int idx) const;
    void update();
    void updateOne();

private:
    int m_Pins[Capacity];
    int m_Values[Capacity];
    int m_UpdateIdx;
};

//void testAnalogInBank(AnalogInBank &bank, int repeats = -1);
