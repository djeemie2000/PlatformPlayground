#include "GateIn.h"

GateState::GateState()
: m_Curr(0)
, m_Prev(0)
{}

void GateState::Tick(int Gate)
{
    m_Prev = m_Curr;
    m_Curr = Gate;
}
int GateState::Get() const
{
    return m_Curr;
}
bool GateState::IsRising() const
{
    return m_Curr && !m_Prev;
}
bool GateState::IsFalling() const
{
    return !m_Curr && m_Prev;
}

DebounceState::DebounceState()
 : m_State(0)
 , m_Cntr(0)
 , m_Maximum(1)//no debouncing
{}

DebounceState::DebounceState(int maximum)
 : m_State(0)
 , m_Cntr(0)
 , m_Maximum(maximum)//debouncing
{}

void DebounceState::SetMaximum(int maximum)
{
    m_Maximum = maximum;
}

void DebounceState::Tick(int Gate)
{
    if(Gate)
    {
        if(m_Cntr<m_Maximum)
        {
            ++m_Cntr;
        }
    }
    else if(0<m_Cntr)//gate == 0
    {
        --m_Cntr;
    }

    if(0==m_Cntr)
    {
        m_State = 0;
    } 
    else if(m_Maximum==m_Cntr)
    {
        m_State = 1;
    }
    // else: state unchanged
}

int DebounceState::Get() const
{
    return m_State;
}

GateIn::GateIn(PinName pin)
: m_In(pin, PullDown)
, m_State()
{}

void GateIn::Read()
{
    m_State.Tick(m_In);
}
int GateIn::Get() const
{
    return m_State.Get();
}
bool GateIn::IsRising() const
{
    return m_State.IsRising();
}
bool GateIn::IsFalling() const
{
    return m_State.IsFalling();
}

ButtonIn::ButtonIn(PinName pin, int maximum)
: m_In(pin, PullDown)
, m_DebounceState(maximum)
, m_State()
{}

void ButtonIn::Read()
{
    m_DebounceState.Tick(m_In);
    m_State.Tick(m_DebounceState.Get());
}

int ButtonIn::Get() const
{
    return m_State.Get();
}

bool ButtonIn::IsRising() const
{
    return m_State.IsRising();
}

bool ButtonIn::IsFalling() const
{
    return m_State.IsFalling();
}

ToggleIn::ToggleIn(PinName pin)
: m_In(pin, PullDown)
, m_State()
, m_ToggleState()
{}

void ToggleIn::Read()
{
    m_State.Tick(m_In);
    if(m_State.IsRising())
    {
        m_ToggleState.Tick(1-m_ToggleState.Get());
    }
}
int ToggleIn::Get() const
{
    return m_ToggleState.Get();
}
bool ToggleIn::IsRising() const
{
    return m_ToggleState.IsRising();
}
bool ToggleIn::IsFalling() const
{
    return m_ToggleState.IsFalling();
}

ToggleInOut::ToggleInOut(PinName inPin, PinName outPin)
: m_In(inPin, PullDown)
, m_Out(outPin)
, m_State()
, m_ToggleState()
{}

void ToggleInOut::Read()
{
    m_State.Tick(m_In);
    if(m_State.IsRising())
    {
        m_ToggleState.Tick(1-m_ToggleState.Get());
    }
    m_Out = m_ToggleState.Get();
}
int ToggleInOut::Get() const
{
    return m_ToggleState.Get();
}
bool ToggleInOut::IsRising() const
{
    return m_ToggleState.IsRising();
}
bool ToggleInOut::IsFalling() const
{
    return m_ToggleState.IsFalling();
}
