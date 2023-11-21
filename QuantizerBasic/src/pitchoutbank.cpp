#include "pitchoutbank.h"
#include "mcp4822bank.h"

PitchOutBank::PitchOutBank() : m_Bank(0)
{}

void PitchOutBank::Begin(MCP4822Bank* bank)
{
    m_Bank = bank;
}

void PitchOutBank::PitchOut(int idx, uint8_t baseMidiNote, uint8_t midiNote)
{
    int val = midiNote - baseMidiNote;
    if(val<0)
    { 
        val = 0;
    }

    //pitch
    // 1000 mV = 12 midi notes
    // 1000/12 equals 250/3
    // 127 * 250 < 2 pow 7 * 2 pow 8 = 2 pow 15 is within int range
    val = val * 250 / 3;
    if(4095<val)
    {
        val = 4095;
    }
    m_Bank->SetValue(idx, val);
}

void TestPitchOut(PitchOutBank& out, int channel)
{
    Serial.println("Test pitch out...");
    for(uint8_t pitch = 0; pitch<49; ++pitch)
    {
        Serial.println(pitch);
        out.PitchOut(channel, 0, pitch);
        delay(500);
    }
    Serial.println("Done");
}

void TestPitchOut2(PitchOutBank& out, int channel)
{
    Serial.println("Test pitch out 2...");
    int notes[] = {0, 2, 4, 5, 7, 9, 11};
    for(int octave = 0; octave<5; ++octave)
    {
        for(int idx = 0; idx<7; ++idx)
        {
            int pitch = 12*octave + notes[idx];
     
            Serial.println(pitch);
            out.PitchOut(channel, 0, pitch);
            delay(500);
        }
    }
    Serial.println("Done");
}
