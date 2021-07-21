#include "DigitalIn.h"

DigitalIn::DigitalIn()
{}

void DigitalIn::begin(int pin)
{
    m_Pin = pin;
    pinMode(m_Pin, INPUT_PULLUP);
    m_Curr = 0;
    m_Prev = 0;
}

void DigitalIn::Read()
{
    m_Prev = m_Curr;
    m_Curr = digitalRead(m_Pin);
}

int DigitalIn::Get() const
{
    return m_Curr;
}

bool DigitalIn::IsRising() const
{
    return m_Curr && !m_Prev;
}

bool DigitalIn::IsFalling() const
{
    return m_Prev && !m_Curr;
}
