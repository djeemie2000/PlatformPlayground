#include "ledout.h"

LedOut::LedOut() 
: m_LedOutPin(-1)
, m_LedOutMask(0x00)
{}

void LedOut::Begin(uint8_t ledOutPin)
{
    m_LedOutPin = ledOutPin;
    pinMode(m_LedOutPin, OUTPUT);
}

void LedOut::LedOn()
{
    m_LedOutMask = 0xFF;
}
    
void LedOut::LedOff()
{
    m_LedOutMask = 0x00;
}

void LedOut::LedBlink()
{
    m_LedOutMask = 0x40;
}

void LedOut::LedBlinkFast()
{
    m_LedOutMask = 0x10;
}

void LedOut::LedBlinkSlow()
{
    m_LedOutMask = 0x80;
}

void LedOut::Update(uint8_t counter)
{
    digitalWrite(m_LedOutPin, m_LedOutMask == 0xFF ? 1 : m_LedOutMask & counter);
}
