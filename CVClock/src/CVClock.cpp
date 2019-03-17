#include "CVClock.h"

CVClock::CVClock(int clockInPin, int gateOutPin, int durationCVPin)
    : m_ClockInPin(clockInPin), m_GateOutPin(gateOutPin),
      m_DurationCVPin(durationCVPin),
      m_Duration(CVClockState::DurationScale / 2), m_State() {}

void CVClock::Begin() {
  pinMode(m_ClockInPin, INPUT);
  pinMode(m_GateOutPin, OUTPUT);
  pinMode(m_DurationCVPin, INPUT);
}

void CVClock::Tick() {
  // put your main code here, to run repeatedly:
  int clockIn = digitalRead(m_ClockInPin); // TODO fast digital read!
  m_Duration = analogRead(m_DurationCVPin);
  int gateOut = m_State.Tick(clockIn, m_Duration);
  digitalWrite(m_GateOutPin, gateOut);
}

void CVClock::debugOut(uint32_t elapsed) {
  Serial.print(elapsed);
  Serial.print(" : ");
  Serial.print(m_State.Period());
  Serial.print(" ");
  Serial.println(m_Duration);
}
