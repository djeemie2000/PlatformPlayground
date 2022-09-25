#include "analogbuttonin2.h"

AnalogButtonIn2::AnalogButtonIn2()
    : m_Button1(false), m_Button2(false)
{
}

void AnalogButtonIn2::Begin(uint8_t analogInPin)
{
    m_AnalogInPin = analogInPin;
}

void AnalogButtonIn2::update()
{
    m_PrevButton1 = m_Button1;
    m_PrevButton2 = m_Button2;

    // value [0, 8[
    m_Value = analogRead(m_AnalogInPin);
    int value = m_Value >> 7;
    if (value < 1)
    {
        m_Button1 = false;
        m_Button2 = false;
    }
    else if (value < 3)
    {
        m_Button1 = true;
        m_Button2 = false;
    }
    else if (value < 5)
    {
        m_Button1 = false;
        m_Button2 = true;
    }
    else
    {
        m_Button1 = true;
        m_Button2 = true;
    }
}

void TestAnalogButtonIn2(AnalogButtonIn2 &in, int repeats)
{
    for (int idx = 0; idx < repeats; ++idx)
    {
        in.update();

        Serial.print(idx);
        Serial.print(' ');
        Serial.print(in.GetValue());
        Serial.print(' ');
        Serial.print(in.Get1());
        Serial.print(' ');
        Serial.println(in.Get2());

        delay(100);
    }
}
