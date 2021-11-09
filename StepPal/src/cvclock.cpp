#include "cvclock.h"

CVClock::CVClock()
    : m_ClockInPin(-1), m_GateOutPin(-1), m_DurationCVPin(-1),
      m_Duration(CVClockState::DurationScale / 2), m_State() {}

void CVClock::Begin(int clockInPin, int gateOutPin, int durationCVPin) {
  m_ClockInPin = clockInPin;
  m_GateOutPin = gateOutPin;
  m_DurationCVPin = durationCVPin;
  // TODO pass pins here...
  pinMode(m_ClockInPin, INPUT);
  pinMode(m_GateOutPin, OUTPUT);
  pinMode(m_DurationCVPin, INPUT);
}

void CVClock::Tick() {
  int clockIn = digitalRead(m_ClockInPin); // TODO fast digital read!

  int gateOut = m_State.Tick(clockIn, m_Duration);
  // read duration in separate function, so can alternate
  digitalWrite(m_GateOutPin, gateOut); // TODO fast digital write!
}

void CVClock::ReadDuration() {
  m_Duration = analogRead(m_DurationCVPin);
  // no duration / full duration
  if (m_Duration < 3) {
    m_Duration = 0;
  } else if (1018 < m_Duration) {
    m_Duration = 1024;
  }
}

void CVClock::debugOut(uint32_t elapsed) {
  Serial.print(" E ");
  Serial.print(elapsed);
  Serial.print(" D ");
  Serial.print(m_Duration);
  m_State.DebugOut();
  Serial.println();
}
