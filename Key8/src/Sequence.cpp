#include "Sequence.h"

Sequence::Sequence() 
: m_Length(0)
{}

void Sequence::Clear()
{
    m_Length = 0;
}

void Sequence::Append(int note, int gate)
{
    if(m_Length<Capacity)
    {
        m_Note[m_Length] = note;
        m_Gate[m_Length] = gate;
        ++m_Length;
    }
}

int Sequence::Length() const
{
    return m_Length;
}

int Sequence::Note(int step) const
{
    // no boundary check!
    return m_Note[step];
}

int Sequence::Gate(int step) const
{
    // no boundary check!
    return m_Gate[step];
}
