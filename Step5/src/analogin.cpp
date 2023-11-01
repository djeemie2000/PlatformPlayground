#include "analogin.h"

AnalogIn::AnalogIn()
{}

void AnalogIn::Begin(int pin)
{
    m_Pin = pin;
    pinMode(m_Pin, INPUT);//needed?
    m_Curr = 0;
    m_Prev = 0;
}

void AnalogIn::Read()
{
    m_Prev = m_Curr;
    m_Curr = analogRead(m_Pin);
}

int AnalogIn::Get() const
{
    return m_Curr;
}

int AnalogIn::Prev() const
{
    return m_Prev;
}

void TestAnalogIn(AnalogIn& analogIn, int repeats)
{
    Serial.println("Testing analog in...");
    for (int repeat = 0; repeat < repeats; ++repeat)
    {
        analogIn.Read();
        analogIn.Read();
        Serial.println(analogIn.Get());
        delay(500);
    }
}

void TestAnalogIn(AnalogIn& analogIn0, AnalogIn& analogIn1, int repeats)
{
    Serial.println("Testing analog ins...");
    for (int repeat = 0; repeat < repeats; ++repeat)
    {
        analogIn0.Read();
        analogIn1.Read();
        Serial.print(analogIn0.Get());
        Serial.print(' ');
        Serial.println(analogIn1.Get());
        delay(500);
    }
}

void TestAnalogIn(AnalogIn& analogIn0, AnalogIn& analogIn1, AnalogIn& analogIn2, int repeats)
{
    Serial.println("Testing analog ins...");
    for (int repeat = 0; repeat < repeats; ++repeat)
    {
        analogIn0.Read();
        analogIn0.Read();
        delay(2);
        analogIn1.Read();
        analogIn1.Read();
        delay(2);
        analogIn2.Read();
        analogIn2.Read();
        delay(2);
        Serial.print(analogIn0.Get());
        Serial.print(' ');
        Serial.print(analogIn1.Get());
        Serial.print(' ');
        Serial.println(analogIn2.Get());
        delay(500);
    }
}
