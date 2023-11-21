#include "digitaloutbank.h"

DigitalOutBank::DigitalOutBank()
{
}

void DigitalOutBank::Begin(uint8_t pin0, uint8_t pin1)
{
    m_OutputPin[0] = pin0;   
    m_OutputPin[1] = pin1;   

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

DigitalOutBank10::DigitalOutBank10()
{
}

void DigitalOutBank10::Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7, uint8_t pin8, uint8_t pin9)
{
    m_OutputPin[0] = pin0;   
    m_OutputPin[1] = pin1;   
    m_OutputPin[2] = pin2;   
    m_OutputPin[3] = pin3;   
    m_OutputPin[4] = pin4;   
    m_OutputPin[5] = pin5;   
    m_OutputPin[6] = pin6;   
    m_OutputPin[7] = pin7;   
    m_OutputPin[8] = pin8;   
    m_OutputPin[9] = pin9;   

    for(int idx = 0; idx<DigitalOutBank10::Size; ++idx)
    {
        pinMode(m_OutputPin[idx], OUTPUT);
        digitalWrite(m_OutputPin[idx], LOW);
    }
}

 void DigitalOutBank10::Set(int idx)
 {
     digitalWrite(m_OutputPin[idx], HIGH);
 }

void DigitalOutBank10::Clear(int idx)
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

void AllClear(DigitalOutBank& bank)
{
    for(int idx = 0; idx<DigitalOutBank::Size;++idx)
    {
        bank.Clear(idx);
    }
}

void TestDigitalOutBank(DigitalOutBank10& bank, int repeats)
{
    Serial.print("Testing digital out bank...");
    const int delayMSecs = 300;
    for(int repeat = 0; repeat<repeats;++repeat)
    {
        for(int idx = 0; idx<DigitalOutBank10::Size;++idx)
        {
            bank.Set(idx);
            delay(delayMSecs);
            bank.Clear(idx);
        }
    }
    Serial.println(" done");
}

void AllClear(DigitalOutBank10& bank)
{
    for(int idx = 0; idx<DigitalOutBank10::Size;++idx)
    {
        bank.Clear(idx);
    }
}
