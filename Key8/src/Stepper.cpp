#include "Stepper.h"

Stepper::Stepper() 
: m_Step(0)
{}

void Stepper::Reset()
{
    m_Step = 0;//-1??
}

void Stepper::Advance(int length)
{
    ++m_Step;
    if(length<=m_Step)
    {
        m_Step = 0;
    }
}

int Stepper::GetStep() const
{
    return m_Step;
}
