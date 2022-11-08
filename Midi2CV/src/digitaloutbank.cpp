#include "digitaloutbank.h"

DigitalOutBank::DigitalOutBank()
{
}

void DigitalOutBank::Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, 
                uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7)
{
    m_OutputPin[0] = pin0;   
    m_OutputPin[1] = pin1;   
    m_OutputPin[2] = pin2;   
    m_OutputPin[3] = pin3;   
    m_OutputPin[4] = pin4;   
    m_OutputPin[5] = pin5;   
    m_OutputPin[6] = pin6; 
    m_OutputPin[7] = pin7;  

    for(int idx = 0; idx<DigitalOutBank::Size; ++idx)
    {
        pinMode(m_OutputPin[idx], OUTPUT);
        digitalWrite(m_OutputPin[idx], LOW);
    }
}

 void DigitalOutBank::Set(int idx)
 {
     digitalWrite(m_OutputPin[idx], HIGH);
 }

void DigitalOutBank::Clear(int idx)
{
    digitalWrite(m_OutputPin[idx], LOW);
}
