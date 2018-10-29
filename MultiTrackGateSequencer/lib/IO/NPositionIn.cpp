#include "NPositionIn.h"

NPositionIn::NPositionIn(PinName pin, int NumPositions)
: m_In(pin)
, m_NumPositions(NumPositions)
, m_Position(0)
{}

void NPositionIn::Read()
{
    m_Position = m_In.read()*m_NumPositions;
    if(m_Position<0)
    {
        m_Position = 0;
    } 
    else if(m_NumPositions<=m_Position)
    {
        m_Position = m_NumPositions-1;
    }
}

int NPositionIn::Get() const
{
    return m_Position;
}
