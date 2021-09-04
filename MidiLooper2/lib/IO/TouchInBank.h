#pragma once
#include <Arduino.h>

class TouchInBank
{
public:
    static const int Capacity = 8;

    TouchInBank(int pin0 = -1, int pin1 = -1, int pin2 = -1, int pin3 = -1, int pin4 = -1, int pin5 = -1, int pin6 = -1, int pin7 = -1);

    void begin(int lowThreshold, int highThreshold);
    void update();
    
    int get(int idx) const;
    bool IsPushed(int Pad) const;
    bool IsClicked(int Pad) const;
    bool IsReleased(int Pad) const;

    int getValue(int idx);

private:
    int m_Pins[Capacity];

    int m_LowThreshold;
    int m_HighThreshold;

    int m_Values[Capacity];
    int m_State[Capacity];
    int m_PrevState[Capacity];
};

void PrintTouchInBank(TouchInBank &touchPad, HardwareSerial &debugSerial);
void TestTouchInBank(TouchInBank &touchPad, HardwareSerial &debugSerial, int count = -1);
