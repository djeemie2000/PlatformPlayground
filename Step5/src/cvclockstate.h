#pragma once
#include <Arduino.h>

class CVClockState
{
public:
  static const int DurationScale = 1024;

  CVClockState();

  int Tick(int clockIn, int duration);

  int Gate() const { return m_Gate; }
  int Period() const { return m_Period; }
  void DebugOut();

private:
  int m_Cntr;
  int m_Period;
  int m_ClockIn;
  int m_Gate;
};
