#include "midinotemask.h"

MidiNoteMask::MidiNoteMask()
{
    m_Mask[0] = 0;
    m_Mask[1] = 0;
    m_Mask[2] = 0;
    m_Mask[3] = 0;
    m_NumNoteOn = 0;
}

void MidiNoteMask::NoteOn(uint8_t midiNote)
{
    if(!IsNoteOn(midiNote))
    {
        bitSet(m_Mask[midiNote >> 5], midiNote & 0x1F);
        ++m_NumNoteOn;
    }
}

void MidiNoteMask::NoteOff(uint8_t midiNote)
{
    if(IsNoteOn(midiNote))
    {
        bitClear(m_Mask[midiNote >> 5], midiNote & 0x1F);
        --m_NumNoteOn;
    }
}

bool MidiNoteMask::IsNoteOn(uint8_t midiNote) const
{
    return bitRead(m_Mask[midiNote >> 5], midiNote & 0x1F);
}
