#pragma once
#include <Arduino.h>

class AnalogButtonIn2
{
public:
    AnalogButtonIn2();

    void Begin(uint8_t analogInPin);
    void update();

    bool Get1() const { return m_Button1; }
    bool Get2() const { return m_Button2; }
    bool IsClicked1() const { return m_Button1 && !m_PrevButton1; }
    bool IsClicked2() const { return m_Button2 && !m_PrevButton2; }

    int GetValue() const { return m_Value;}

private:
    uint8_t m_AnalogInPin;
    int m_Value;
    bool m_Button1;
    bool m_PrevButton1;
    bool m_Button2;
    bool m_PrevButton2;
};

void TestAnalogButtonIn2(AnalogButtonIn2 &in, int repeats);
