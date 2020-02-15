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

GateIn::GateIn(uint8_t pin, bool pullup)
: m_InPin(pin)//TODO, PullDown)
, m_State()
{
    pinMode(m_InPin, pullup? INPUT_PULLUP : INPUT);
}

void GateIn::Read()
{
    m_State.Tick(digitalRead(m_InPin));
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

ButtonIn::ButtonIn(uint8_t pin, int maximum)
: m_InPin(pin)//TODO, PullDown)
, m_DebounceState(maximum)
, m_State()
{}

void ButtonIn::Read()
{
    m_DebounceState.Tick(digitalRead(m_InPin));
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

ToggleIn::ToggleIn(uint8_t pin)
: m_InPin(pin)//, PullDown)
, m_State()
, m_ToggleState()
{}

void ToggleIn::Read()
{
    m_State.Tick(digitalRead(m_InPin));
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
