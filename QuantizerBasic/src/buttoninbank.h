#pragma once
#include <Arduino.h>

class ButtonInBank
{
public:
    static const int Size = 4;

    ButtonInBank();

    void Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3);
    void SetDebounce(int debounce);

    bool Get(int idx) const;
    bool IsClicked(int idx) const;
    bool IsReleased(int idx) const;

    void Update(unsigned long millis);

private:
    unsigned long m_Debounce;
    unsigned long m_Millis;
    bool m_Sampled;
    uint8_t m_InputPin[Size];
    uint8_t m_History[Size];
};

void TestButtonIn(ButtonInBank& buttonIn, int repeats);
void TestButtonInGet(ButtonInBank& buttonIn, int repeats);
