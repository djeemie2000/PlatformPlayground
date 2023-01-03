#include "AnalogIn.h"

AnalogIn::AnalogIn()
{}

void AnalogIn::begin(int pin)
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
