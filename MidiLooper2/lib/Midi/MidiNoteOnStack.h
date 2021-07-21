#pragma once
#include <Arduino.h>

class MidiNoteOnStack
{
public:
    MidiNoteOnStack();

    uint16_t GetNoteOn(uint8_t midiNote) const;
    void SetNoteOn(uint8_t midiNote);
    void SetNoteOff(uint8_t midiNote);

private:
    uint16_t m_Bits[8]; // 128 midi notes = 8*16
};
