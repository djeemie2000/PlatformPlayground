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
//    Serial.println("LedOn");
    m_LedOutMask = 0xFF;
}
    
void LedOut::LedOff()
{
    //Serial.println("LedOff");
    m_LedOutMask = 0x00;
}

void LedOut::LedBlink()
{
    //Serial.println("LedBlink");
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


