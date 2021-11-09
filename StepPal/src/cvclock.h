#pragma once

#include <Arduino.h>
#include "cvclockstate.h"


class CVClock {
public:
  CVClock();

  void Begin(int clockInPin, int gateOutPin, int durationCVPin);
  void Tick();
  void ReadDuration();

  void debugOut(uint32_t elapsed);

private:
  int m_ClockInPin;
  int m_GateOutPin;
  int m_DurationCVPin;
  int m_Duration;
  CVClockState m_State;
};
