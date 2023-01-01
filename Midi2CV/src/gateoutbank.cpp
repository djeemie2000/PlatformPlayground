#include "gateoutbank.h"
#include "digitaloutbank.h"
#include "mcp4822bank.h"

GateOutBank::GateOutBank()
 : m_Counter(0x00)
{
}

void GateOutBank::Begin()
{
     for(int idx = 0; idx<GateOutBank::Size; ++idx)
    {
        m_Gate[idx] = 0;
    }

    m_Counter = 0x00;
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
    // counter for triggers ? store previous counter to check if is is changed??
    if(m_Counter != counter)
    {
        for(int idx = 0; idx<GateOutBank::Size; ++idx)
        {
            if(m_Gate[idx] && m_Gate[idx] != 0xFF)
            {
                --m_Gate[idx];
            }
        }
        m_Counter = counter;
    }
}

void GateOutBank::Apply(DigitalOutBank& bank)
{
    for(int idx = 0; idx<GateOutBank::Size; ++idx)
    {
        if(m_Gate[idx])
        {
            bank.Set(idx);
        }
        else
        {
            bank.Clear(idx);
        }
    }
}

void GateOutBank::Apply(int offset, DigitalOutBank& bank, MCP4822Bank& abank)
{
    int halfSize = GateOutBank::Size/2;
    for(int idx = 0; idx<halfSize; ++idx)
    {
        if(m_Gate[2*idx])
        {
            abank.SetValue(offset+idx, 4095);
        }
        else
        {
            abank.SetValue(offset+idx, 0);
        }

        if(m_Gate[2*idx+1])
        {
            bank.Set(offset+idx);
        }
        else
        {
            bank.Clear(offset+idx);
        }
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
