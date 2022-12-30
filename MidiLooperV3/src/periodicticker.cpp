#include "periodicticker.h"

PeriodicTicker::PeriodicTicker()
    : m_Period(2), m_Cntr(0), m_DoReset(true), m_Flag(false)
{
}

void PeriodicTicker::Tick()
{
    if (m_DoReset)
    {
        m_Cntr = 0;
        m_DoReset = false;
    }
    else
    {
        ++m_Cntr;
        if (m_Period <= m_Cntr)
        {
            m_Cntr = 0;
        }
    }

    if (m_Cntr == 0)
    {
        Set();
    }
}
