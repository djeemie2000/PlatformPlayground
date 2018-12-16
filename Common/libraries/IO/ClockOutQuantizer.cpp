#include "ClockOutQuantizer.h"

ClockOutQuantizer::ClockOutQuantizer() 
:  m_Duration(8)
, m_State()
{}

void ClockOutQuantizer::Tick(int segment)
{
    if(m_State.Get())
    {
        // 1 -> 0 ?
        if(m_Duration<segment)
        {
            m_State.Tick(0);
        }
    }
    else
    {
        // 0 -> 1 ?
        if(segment==0)
        {
            m_State.Tick(1);
        }
    }
}

int ClockOutQuantizer::Get() const
{
    return m_State.Get();
}

bool ClockOutQuantizer::IsRising() const
{
    return m_State.IsRising();
}

bool ClockOutQuantizer::IsFalling() const
{
    return m_State.IsFalling();
}

void ClockOutQuantizer::SetDuration(int duration)
{
    m_Duration = duration;
}
