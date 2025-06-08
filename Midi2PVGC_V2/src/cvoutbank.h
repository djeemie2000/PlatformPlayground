#pragma once
#include <Arduino.h>

class MCP4728Dac;

template<int Size>
class CVOutBank
{
public:
    CVOutBank();

    void Begin();

    void SetPitchMode(int idx);
    void SetVelocityMode(int idx);

    void PitchOut(int idx, uint8_t baseMidiNote, uint8_t midiNote);
    void VelocityOut(int idx, uint8_t velocity);

    void Apply(MCP4728Dac& bank);

private:
    uint8_t m_VelocityMask;
    uint8_t m_Value[Size];
};

#include "cvoutbank.h"
#include "mcp4728.h"

template<int Size>
CVOutBank<Size>::CVOutBank()
    : m_VelocityMask(0x00)
{
}

template<int Size>
void CVOutBank<Size>::Begin()
{
    for (int idx = 0; idx < Size; ++idx)
    {
        m_Value[idx] = 0;
    }
    m_VelocityMask = 0x00;
}

template<int Size>
void CVOutBank<Size>::SetPitchMode(int idx)
{
    bitClear(m_VelocityMask, idx);
}

template<int Size>
void CVOutBank<Size>::SetVelocityMode(int idx)
{
    bitSet(m_VelocityMask, idx);
}

template<int Size>
void CVOutBank<Size>::PitchOut(int idx, uint8_t baseMidiNote, uint8_t midiNote)
{
    if (baseMidiNote < midiNote)
    {
        m_Value[idx] = midiNote - baseMidiNote;
    }
    else
    {
        m_Value[idx] = 0x00;
    }
    bitClear(m_VelocityMask, idx);
}

template<int Size>
void CVOutBank<Size>::VelocityOut(int idx, uint8_t velocity)
{
    m_Value[idx] = velocity;

    bitSet(m_VelocityMask, idx);
}

template<int Size>
void CVOutBank<Size>::Apply(MCP4728Dac& bank)
{
    for(int channel = 0; channel<Size; ++channel)
    {
        if(bitRead(m_VelocityMask, channel))
        {
            //velocity
            bank.ConfigureChannel(channel, MCP4728Dac::MCP4728_VREF_VDD, MCP4728Dac::MCP4728_GAIN_1X);
            // 0-127 (7 bits) to 0-4095 (12 bits)
            int val = m_Value[channel];
            bank.SetValue(channel, val << 5);
        }
        else
        {
            //pitch
            bank.ConfigureChannel(channel, MCP4728Dac::MCP4728_VREF_INTERNAL, MCP4728Dac::MCP4728_GAIN_2X);
            // 1000 mV = 12 midi notes
            // 1000/12 equals 250/3
            // 127 * 250 < 2 pow 7 * 2 pow 8 = 2 pow 15 is within int range
            int val = m_Value[channel];
            val = val * 250 / 3;
            if(4095<val)
            {
                val = 4095;
            }
            bank.SetValue(channel, val);
        }
    }
}
