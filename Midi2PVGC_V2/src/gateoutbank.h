#pragma once
#include <Arduino.h>

template<int Size>
class FastDigitalOutBank;

template<int Size>
class GateOutBank
{
public:
    GateOutBank();

    void Begin();

    void GateOn(int idx);
    void GateOff(int idx);
    void Trigger(int idx);

    void Update(uint8_t counter); // counter for triggers
    void Apply(FastDigitalOutBank<Size>& bank);
    void Apply(int offset, FastDigitalOutBank<Size>& bank);

    void PrintState();

private:
    uint8_t m_Gate[Size];//TODO uint8_t m_State;
    uint8_t m_Counter;
};

template<int Size>
void AllGateOff(GateOutBank<Size>& bank);

template<int Size>
void SetGate(GateOutBank<Size>& bank, int idx, int gate);

#include "gateoutbank.h"
#include "fastdigitaloutbank.h"

template<int Size>
GateOutBank<Size>::GateOutBank()
 : m_Counter(0x00)
{
}

template<int Size>
void GateOutBank<Size>::Begin()
{
    for(int idx = 0; idx<Size; ++idx)
    {
        m_Gate[idx] = 0;
    }

    m_Counter = 0x00;
}

template<int Size>
void GateOutBank<Size>::GateOn(int idx)
{
    m_Gate[idx] = 0xFF;
}

template<int Size>
void GateOutBank<Size>::GateOff(int idx)
{
    m_Gate[idx] = 0;
}

template<int Size>
void GateOutBank<Size>::Trigger(int idx)
{
    //TODO gate to some value???
    m_Gate[idx] = 4;
}

template<int Size>
void GateOutBank<Size>::Update(uint8_t counter)
{
    // counter for triggers ? store previous counter to check if is is changed??
    if(m_Counter != counter)
    {
        for(int idx = 0; idx<Size; ++idx)
        {
            if(m_Gate[idx] && m_Gate[idx] != 0xFF)
            {
                --m_Gate[idx];
            }
        }
        m_Counter = counter;
    }
}

template<int Size>
void GateOutBank<Size>::Apply(FastDigitalOutBank<Size>& bank)
{
    Apply(0, bank);
}

template<int Size>
void GateOutBank<Size>::Apply(int offset, FastDigitalOutBank<Size>& bank)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        if(m_Gate[idx])
        {
            bank.Set(offset+idx);
        }
        else
        {
            bank.Clear(offset+idx);
        }
    }
    bank.Update();
}

template<int Size>
void GateOutBank<Size>::PrintState()
{
    for(int idx=0; idx<Size; ++idx)
    {
        Serial.print(m_Gate[idx], HEX);
        Serial.print(' ');
    }
    Serial.println();
}


template<int Size>
void AllGateOff(GateOutBank<Size>& bank)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        bank.GateOff(idx);
    }
}

template<int Size>
void SetGate(GateOutBank<Size>& bank, int idx, int gate)
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
