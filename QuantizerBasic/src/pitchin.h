#pragma once
#include <Arduino.h>

#include "analogin.h"

class PitchIn
{
public:
    PitchIn();

    void Begin(int pin);
    void Read();
    int Get() const { return m_Pitch;   }
    int Prev() const { return m_PrevPitch; }

private:
    AnalogIn m_In;
    int m_Pitch;
    int m_PrevPitch;
};
