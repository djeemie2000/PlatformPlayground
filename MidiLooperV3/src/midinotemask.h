#pragma once
#include <Arduino.h>

class MidiNoteMask
{
public:
    MidiNoteMask();

    void NoteOn(uint8_t midiNote);
    void NoteOff(uint8_t midiNote);
    bool IsNoteOn(uint8_t midiNote) const;
    int NumNoteOn() const { return m_NumNoteOn;}

private:
    uint32_t m_Mask[4];
    int m_NumNoteOn;
};
