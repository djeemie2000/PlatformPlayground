#pragma once
#include <Arduino.h>

class ShiftOutBank
{
public:
    static const int Banks = 3; 
    static const int Size = 8;
    //static const int Capacity = Banks * Size;

    ShiftOutBank();

    void begin(int csPin);
    void set(int idxBank, int idx, int value);
    int get(int idxBank, int idx);
    void update();

private:
    int m_CsPin;
    uint8_t m_Values[Banks];
    uint8_t m_ValuesIn[Banks];
};

void testDigitalOutBank(ShiftOutBank &bank, int repeats = -1);
