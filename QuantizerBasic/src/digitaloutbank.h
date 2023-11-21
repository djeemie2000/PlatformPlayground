#pragma once
#include <Arduino.h>

class DigitalOutBank
{
public:
    static const int Size = 2;

    DigitalOutBank();

    void Begin(uint8_t pin0, uint8_t pin1);

    void Set(int idx);
    void Clear(int idx);

private:
    // no state is needed!
    uint8_t m_OutputPin[Size];
};

class DigitalOutBank10
{
public:
    static const int Size = 10;

    DigitalOutBank10();

    void Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7, uint8_t pin8, uint8_t pin9);

    void Set(int idx);
    void Clear(int idx);

private:
    // no state is needed!
    uint8_t m_OutputPin[Size];
};

void TestDigitalOutBank(DigitalOutBank& bank, int repeats);
void AllClear(DigitalOutBank& bank);
void TestDigitalOutBank(DigitalOutBank10& bank, int repeats);
void AllClear(DigitalOutBank10& bank);
