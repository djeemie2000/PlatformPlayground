#pragma once

#include <Arduino.h>
#include "CVClockState.h"

class CVClock
{
  public:
    CVClock(int clockInPin, int gateOutPin, int durationCVPin);

    void Begin();
    void Tick();

    void debugOut(uint32_t elapsed);

  private:
    const int m_ClockInPin;
    const int m_GateOutPin;
    const int m_DurationCVPin;
    int m_Duration;
    CVClockState m_State;
};
