#include "digitalin.h"

DigitalIn::DigitalIn()
{}

void DigitalIn::begin(int pin, bool pullup)
{
    m_Pin = pin;
    pinMode(m_Pin, pullup ? INPUT_PULLUP : INPUT);
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
