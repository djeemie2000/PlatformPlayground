#pragma once
#include <Arduino.h>

class CVOutBank
{
public:
    static const int Size = 8;

    CVOutBank();

    void Begin();

    void SetPitchMode(int idx);
    void SetVelocityMode(int idx);

    void PitchOut(int idx, uint8_t baseMidiNote, uint8_t midiNote);
    void VelocityOut(int idx, uint8_t velocity);

    //TODO Apply() to MCP4822 x2 / to MCP4728 DAC

private:
    // TODO state!
    uint8_t m_VelocityMask;
    uint8_t m_Value[Size];
};