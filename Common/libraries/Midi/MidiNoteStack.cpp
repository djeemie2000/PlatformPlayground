#include "MidiNoteStack.h"
#include <algorithm>

MidiNoteStack::MidiNoteStack() : m_Stack(), m_Size(0) {}

void MidiNoteStack::NoteOn(uint8_t MidiNote)
{
    if (m_Size < Capacity)
    {
        // no check if note is already in stack!
        m_Stack[m_Size] = MidiNote;
        ++m_Size;
    }
}

void MidiNoteStack::NoteOff(uint8_t MidiNote)
{
    // implicitly checks if note was in stack
    uint8_t *removed = std::remove(m_Stack, m_Stack + m_Size, MidiNote);
    m_Size = removed - m_Stack;
}

uint8_t MidiNoteStack::Pop()
{
    if (0 < m_Size)
    {
        --m_Size;
        return m_Stack[m_Size];
    }
    return 0x00;
}

uint8_t MidiNoteStack::Note(int idx) const
{
    return (0 <= idx && idx < m_Size) ? m_Stack[m_Size - 1 - idx] : 0x00;
}
