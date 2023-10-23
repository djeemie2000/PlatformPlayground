#include "midinotestack.h"
#include <algorithm>

MidiNoteStack::MidiNoteStack() : m_Stack(), m_Size(0) {}

uint16_t MidiNoteStack::Concat(uint8_t midiNote, uint8_t midiChannel)
{
    return midiNote | midiChannel<<8;
}

void MidiNoteStack::NoteOn(uint8_t midiNote, uint8_t midiChannel)
{
    if (m_Size < Capacity)
    {
        // no check if note is already in stack!
        m_Stack[m_Size] = Concat(midiNote, midiChannel);
        ++m_Size;
    }
}

void MidiNoteStack::NoteOff(uint8_t midiNote, uint8_t midiChannel)
{
    // implicitly checks if note was in stack
    uint16_t *removed = std::remove(m_Stack, m_Stack + m_Size, Concat(midiNote, midiChannel));
    m_Size = removed - m_Stack;
}

bool MidiNoteStack::Contains(uint8_t midiNote, uint8_t midiChannel)
{
    return std::find(m_Stack, m_Stack+m_Size, Concat(midiNote, midiChannel))!=(m_Stack+m_Size);
}

uint16_t MidiNoteStack::Pop()
{
    if (0 < m_Size)
    {
        --m_Size;
        return m_Stack[m_Size];
    }
    return 0x00;
}

uint16_t MidiNoteStack::Get(int idx) const
{
    return (0 <= idx && idx < m_Size) ? m_Stack[m_Size - 1 - idx] : 0x00;
}

uint8_t MidiNoteStack::ToNote(uint16_t value)
{
    return value & 0xFF;
}

uint8_t MidiNoteStack::ToChannel(uint16_t value)
{
    return value >> 8;
}

