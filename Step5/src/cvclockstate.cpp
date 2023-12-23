#include "cvclockstate.h"

CVClockState::CVClockState()
    : m_Cntr(0), m_Period(500), m_ClockIn(0), m_Gate(0) {}

int CVClockState::Tick(int clockIn, int duration)
{
  if (!m_ClockIn && clockIn)
  {
    // clock rising edge
    // => measure period
    if (0 < m_Cntr)
    {
      m_Period = m_Cntr;
    }
    // => reset counter
    m_Cntr = 0;
    // => update gate
    m_Gate = duration ? 1 : 0;
  }
  else if (m_Gate && duration < DurationScale)
  {
    // gate off when cntr > period * duration/durationscale
    // int32_t to prevent overflow
    int32_t lDuration = duration;
    int32_t lCntr = m_Cntr;
    if (m_Period * lDuration < lCntr * DurationScale)
    {
      m_Gate = 0;
    }
  }

  m_ClockIn = clockIn;
  ++m_Cntr;

  return m_Gate;
}

void CVClockState::DebugOut()
{
  Serial.print(" P ");
  Serial.print(m_Period);
  Serial.print(" C ");
  Serial.print(m_Cntr);
  Serial.print(" G ");
  Serial.print(m_Gate);
}