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

GateIn:: GateIn(PinName pin)
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
