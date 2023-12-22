#include "cvclock.h"

CVClock::CVClock()
: m_State(0)
{}

void CVClock::Begin()
{
    m_State = 0;
}

void CVClock::Update(int clockIn, int cv)
{
    if(cv<32)
    {
        m_State = 0;
    }
    else if(cv>992)//1024-32)
    {
        m_State = 1;
    }
    else
    {
        m_State = (0<clockIn) ? 1 : 0;
    }
}

int CVClock::Get() const
{
    return m_State;
}
