#include "ClockInQuantizer.h"

ClockInQuantizer::ClockInQuantizer(int numSegments)
 : m_NumSegments(numSegments)
 , m_Cntr(0)
 , m_Period(1)
 {}

void ClockInQuantizer::Tick(bool reset, bool updatePeriodUponReset)
{
    if(reset)
    {
        if(updatePeriodUponReset && m_Cntr)//never period == 0
        {
            m_Period = m_Cntr;
        }
        m_Cntr = 0;
    }
    else
    {
        ++m_Cntr;
    }
}

int ClockInQuantizer::GetSegment() const
{
    //assumes m_Period != 0 !!
    return m_Cntr*m_NumSegments/m_Period;
}

int ClockInQuantizer::GetNumSegments() const
{
    return m_NumSegments;
}
