#pragma once
#include <Arduino.h>

class AnalogOutBank
{
public:
    static const int Capacity = 6;

    AnalogOutBank(int size);

    void begin();
    void set(int idx, int value);
    void update();
    int size() const { return m_Size;}

private:
   int m_Size;
   int m_Values[Capacity];
};

void testAnalogOutBank(AnalogOutBank &bank, int repeats = -1);
