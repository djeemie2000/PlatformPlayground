#include "ledout.h"

LedOut::LedOut() 
: m_LedOutMask(0x00)
{}

void LedOut::Begin()
{
    m_LedOutMask = 0x00;
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

void LedOut::Apply(uint8_t counter, uint8_t ledOutPin)
{
    //DigitalOut class?
    digitalWrite(ledOutPin, m_LedOutMask == 0xFF ? 1 : m_LedOutMask & counter);
}