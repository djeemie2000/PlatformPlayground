#include "pitchin.h"

PitchIn::PitchIn() : m_In(), m_Pitch(0)
{}

void PitchIn::Begin(int pin)
{
    m_In.Begin(pin);
    m_Pitch = 0;
    m_PrevPitch = 0;
}

void PitchIn::Read()
{
    m_In.Read();

    m_PrevPitch = m_Pitch;
    
    const int margin = 4;
    int value = m_In.Get() + margin;

    // 1024 ~ 5V ~ midi note 60
    // => midi note = 60 * value / 1024 = 15 * value / 256
    m_Pitch = (value * 15) >> 8;

    // no idea why but need to add 3, perhaps diode voltage drop ??
    const int correction = 0;//3; 
    m_Pitch += correction;
}
