#pragma once

#include <Arduino.h>

class CVClockState {
public:
  static const int DurationScale = 1024;

  CVClockState();

  int Tick(int clockIn, int duration);

  int Period() const { return m_Period; }

private:
  int m_Cntr;
  int m_Period;
  int m_ClockIn;
  int m_Gate;
};
