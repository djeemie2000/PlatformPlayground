#include "MidiNoteStack.h"

MidiNoteStack::MidiNoteStack()
 : m_Stack()
 , m_Size(0)
{}

void MidiNoteStack::NoteOn(uint8_t MidiNote)
{
    if(m_Size<Capacity)
    {
        //no check if note is already in stack!
        m_Stack[m_Size] = MidiNote;
        ++m_Size;
    }
}

void MidiNoteStack::NoteOff(uint8_t MidiNote)
{
    // implicitly checks if note was in stack
    uint8_t* removed = std::remove(m_Stack, m_Stack+m_Size, MidiNote);
    m_Size = removed - m_Stack;
}
