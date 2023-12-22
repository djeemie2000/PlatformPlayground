#include "fakeclock.h"

FakeClock::FakeClock()
    : m_Counter(0), m_Period(500)
{
}

void FakeClock::Begin(int period)
{
    m_Period = period;
    m_Counter = 0;
}

void FakeClock::Tick()
{
    ++m_Counter;
    if(m_Period<=m_Counter)
    {
        m_Counter = 0;
    }
}

bool FakeClock::IsClicked() const
{
    return 0 == m_Counter;
}

int FakeClock::Get() const
{
    return m_Counter < m_Period / 2;
}
