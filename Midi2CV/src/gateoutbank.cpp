#include "gateoutbank.h"

GateOutBank::GateOutBank()
{
}

void GateOutBank::Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, 
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

     for(int idx = 0; idx<GateOutBank::Size; ++idx)
    {
        m_Gate[idx] = 0;
        pinMode(m_OutputPin[idx], OUTPUT);
        digitalWrite(m_OutputPin[idx], m_Gate[idx]);
    }
}

void GateOutBank::GateOn(int idx)
{
    m_Gate[idx] = 0xFF;
}

void GateOutBank::GateOff(int idx)
{
    m_Gate[idx] = 0;
}

void GateOutBank::Trigger(int idx)
{
    //TODO gate to some value???
    m_Gate[idx] = 4;
}

void GateOutBank::Update(uint8_t counter)
{
    // TODO counter for triggers ? store previous counter to check if is is changed??

    for(int idx = 0; idx<GateOutBank::Size; ++idx)
    {
        if(m_Gate[idx] && m_Gate[idx] != 0xFF)
        {
            --m_Gate[idx];
        }
        digitalWrite(m_OutputPin[idx], m_Gate[idx]);
    }
}

void GateOutBank::PrintState()
{}


void AllGateOff(GateOutBank& bank)
{
    for(int idx = 0; idx<GateOutBank::Size; ++idx)
    {
        bank.GateOff(idx);
    }
}

void SetGate(GateOutBank& bank, int idx, int gate)
{
    if(gate)
    {
        bank.GateOn(idx);
    }
    else
    {
        bank.GateOff(idx);
    }
}
