#include "NPositionIn.h"

NPositionIn::NPositionIn(PinName pin, int NumPositions)
: m_In(pin)
, m_NumPositions(NumPositions)
{}

void NPositionIn::Read()
{
    m_Position = m_In.read()/m_NumPositions;
}

int NPositionIn::Get() const
{
    return m_Position;
}
