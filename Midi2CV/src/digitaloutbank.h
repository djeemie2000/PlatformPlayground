#pragma once
#include <Arduino.h>

class DigitalOutBank
{
public:
    static const int Size = 8;

    DigitalOutBank();

    void Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, 
                uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7);

    void Set(int idx);
    void Clear(int idx);

private:
    // no state is needed!
    uint8_t m_OutputPin[Size];
};

void TestDigitalOutBank(DigitalOutBank& bank, int repeats);
