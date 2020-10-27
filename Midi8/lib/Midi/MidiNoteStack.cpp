#include "MidiNoteStack.h"
//#include <iterator>
//#include <algorithm>

// template< class ForwardIt, class T >
// ForwardIt remove2(ForwardIt first, ForwardIt last, const T& value)
// {
//     //first = std::find(first, last, value);
//     if (first != last)
//         for(ForwardIt i = first; ++i != last; )
//             if (!(*i == value))
//                 *first++ = *i;
//     return first;
// }//TODO!!!!!!!!!!!!!!!!!!

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
    //uint8_t *removed = remove2(m_Stack, m_Stack + m_Size, MidiNote);
    //m_Size = removed - m_Stack;
    uint8_t* rptr = m_Stack;
    uint8_t* wptr = m_Stack;
    uint8_t* end = m_Stack+m_Size;
    while(rptr<end)
    {
        if(rptr != wptr)
        { 
            *wptr = *rptr;
        }
        if(*rptr == MidiNote)
        {
            --wptr;//++rptr;
            --m_Size;
        }
        ++rptr;
        ++wptr;
    }
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

void MidiNoteStack::Clear()
{
    m_Size = 0;
}

uint8_t MidiNoteStack::Note(int idx) const
{
    return (0 <= idx && idx < m_Size) ? m_Stack[m_Size - 1 - idx] : 0x00;
}

//------

MidiNoteVelocityStack::Item::Item() : m_MidiNote(0x00), m_Velocity(0x00)
{}

MidiNoteVelocityStack::Item::Item(uint8_t midiNote) : m_MidiNote(midiNote), m_Velocity(0x00)
{}

bool MidiNoteVelocityStack::Item::operator==(const Item& other) const
{
    return m_MidiNote == other.m_MidiNote;
}

MidiNoteVelocityStack::MidiNoteVelocityStack() : m_Stack(), m_Size(0) {}

void MidiNoteVelocityStack::NoteOn(uint8_t MidiNote, uint8_t velocity)
{
    if (m_Size < Capacity)
    {
        // no check if note is already in stack!
        m_Stack[m_Size].m_MidiNote = MidiNote;
        m_Stack[m_Size].m_Velocity = velocity;
        ++m_Size;
    }
}

void MidiNoteVelocityStack::NoteOff(uint8_t midiNote)
{
    // implicitly checks if note was in stack
    Item removeItem(midiNote);
    // Item *removed = remove2(m_Stack, m_Stack + m_Size, tmp);
    // m_Size = removed - m_Stack;
    Item* rptr = m_Stack;
    Item* wptr = m_Stack;
    Item* end = m_Stack+m_Size;
    while(rptr<end)
    {
        if(rptr != wptr)
        { 
            *wptr = *rptr;
        }
        if(*rptr == removeItem)
        {
            --wptr;//++rptr;
            --m_Size;
        }
        ++rptr;
        ++wptr;
    }
}

void MidiNoteVelocityStack::Clear()
{
    m_Size = 0;
}

MidiNoteVelocityStack::Item MidiNoteVelocityStack::Pop()
{
    if (0 < m_Size)
    {
        --m_Size;
        return m_Stack[m_Size];
    }
    return Item();
}

uint8_t MidiNoteVelocityStack::Note(int idx) const
{
    return (0 <= idx && idx < m_Size) ? m_Stack[m_Size - 1 - idx].m_MidiNote : 0x00;
}

uint8_t MidiNoteVelocityStack::Velocity(int idx) const
{
    return (0 <= idx && idx < m_Size) ? m_Stack[m_Size - 1 - idx].m_Velocity : 0x00;
}
