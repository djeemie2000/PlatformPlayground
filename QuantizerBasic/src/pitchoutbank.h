#pragma once
#include <Arduino.h>

class MCP4822Bank;

class PitchOutBank
{
public:
    static const int Size = 2;

    PitchOutBank();
    void Begin(MCP4822Bank* bank);
    void PitchOut(int idx, uint8_t baseMidiNote, uint8_t midiNote);

private:
    MCP4822Bank* m_Bank;
};

void TestPitchOut(PitchOutBank& out, int channel);
void TestPitchOut2(PitchOutBank& out, int channel);
