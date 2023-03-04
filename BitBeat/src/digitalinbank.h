#pragma once
#include <Arduino.h>

class DigitalInBank
{
public:
    static const int Size = 2;

    DigitalInBank();

    void Begin(uint8_t pin0, uint8_t pin1);

    bool Get(int idx) const;
    bool IsClicked(int idx) const;
    bool IsReleased(int idx) const;

    void Update(/*unsigned long millis*/);

private:
    uint8_t m_InputPin[Size];
    uint8_t m_History[Size];
};

void TestButtonIn(DigitalInBank& buttonIn, int repeats);
void TestButtonInGet(DigitalInBank& buttonIn, int repeats);