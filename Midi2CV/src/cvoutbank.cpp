#include "cvoutbank.h"

CVOutBank::CVOutBank()
    : m_VelocityMask(0x00)
{
}

void CVOutBank::Begin()
{
    for (int idx = 0; idx < CVOutBank::Size; ++idx)
    {
        m_Value[idx] = 0;
    }
    m_VelocityMask = 0x00;
}

void CVOutBank::SetPitchMode(int idx)
{
    bitClear(m_VelocityMask, idx);
}

void CVOutBank::SetVelocityMode(int idx)
{
    bitSet(m_VelocityMask, idx);
}

void CVOutBank::PitchOut(int idx, uint8_t baseMidiNote, uint8_t midiNote)
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

void CVOutBank::VelocityOut(int idx, uint8_t velocity)
{
    m_Value[idx] = velocity;

    bitSet(m_VelocityMask, idx);
}
