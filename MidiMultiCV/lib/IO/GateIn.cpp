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
: m_In(pin)
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
