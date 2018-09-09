#include "PeriodicOut.h"

  PeriodicOutState::PeriodicOutState() 
  : m_Period(1024)
  , m_Cntr(0)
  , m_Duration(0.5f)
  , m_State(0)
  {}

    void PeriodicOutState::Tick(bool Reset)
    {
        if(Reset)
        {
            m_Cntr = 0;
        }
        m_State = (m_Duration*m_Period<m_Cntr) ? 1 : 0;

        // advance counter
        ++m_Cntr;
        if(m_Period<=m_Cntr)
        {
            m_Cntr = 0;
        }
    }

    int PeriodicOutState::Get() const
    {
        return m_State;
    }

    void PeriodicOutState::SetPeriod(int Period)
    {
        m_Period = Period;
    }
    void PeriodicOutState::SetDuration(float Duration)
    {
        m_Duration = Duration;
    }
    void PeriodicOutState::Reset()
    {
        m_Cntr = 0;
    }
    