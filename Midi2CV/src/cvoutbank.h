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

private:
    // TODO 4x MCP4822 ???
};