#include "digitaloutbank.h"

DigitalOutBank::DigitalOutBank()
{
}

void DigitalOutBank::Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3)
{
    m_OutputPin[0] = pin0;   
    m_OutputPin[1] = pin1;   
    m_OutputPin[2] = pin2;   
    m_OutputPin[3] = pin3;   

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

void TestDigitalOutBank(DigitalOutBank& bank, int repeats)
{
    Serial.print("Testing digital out bank...");
    const int delayMSecs = 300;
    for(int repeat = 0; repeat<repeats;++repeat)
    {
        for(int idx = 0; idx<DigitalOutBank::Size;++idx)
        {
            bank.Set(idx);
            delay(delayMSecs);
            bank.Clear(idx);
        }
    }
    Serial.println(" done");
}
