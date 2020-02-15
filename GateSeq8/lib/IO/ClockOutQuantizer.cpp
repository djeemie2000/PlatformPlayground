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
        else
        {
            m_State.Tick(1);
        }
    }
    else
    {
        // 0 -> 1 ?
        if(segment==0)
        {
            m_State.Tick(1);
        }
        else
        {
            m_State.Tick(0);
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


PeriodicCounter::PeriodicCounter()
: m_Cntr(0)
, m_Period(1)
{}
    
void PeriodicCounter::Tick()
{
    ++m_Cntr;
    if(m_Period<=m_Cntr)
    {
        m_Cntr = 0;
    }
}

void PeriodicCounter::SetPeriod(int period)
{
    m_Period = period;
}

int PeriodicCounter::Cntr() const
{
    return m_Cntr;
}

int PeriodicCounter::Period() const
{
    return m_Period;
}
    
ClockOutState::ClockOutState() 
:  m_Duration(0.5f)
, m_State()
{}

void ClockOutState::Tick(int cntr, int period)
{
    if(m_State.Get())
    {
        // 1 -> 0 or 1 -> 1?
        m_State.Tick(m_Duration*period<cntr?0:1);
    }
    else
    {
        // 0 -> 1 or 0 -> 0 ?
        m_State.Tick(cntr==0?1:0);
    }
}

int ClockOutState::Get() const
{
    return m_State.Get();
}

bool ClockOutState::IsRising() const
{
    return m_State.IsRising();
}

bool ClockOutState::IsFalling() const
{
    return m_State.IsFalling();
}

void ClockOutState::SetDuration(float duration)
{
    m_Duration = duration;
}

