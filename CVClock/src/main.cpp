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

CVClockState::CVClockState()
    : m_Cntr(0), m_Period(500), m_ClockIn(0), m_Gate(0) {}

int CVClockState::Tick(int clockIn, int duration) {
  if (!m_ClockIn && clockIn) {
    // clock rising edge
    // => measure period
    if (0 < m_Cntr) {
      m_Period = m_Cntr;
    }
    // => reset counter
    m_Cntr = 0;
    // => update gate
    m_Gate = duration ? 1 : 0;
  } else if (m_Gate) {
    // gate off when cntr > period * duration/durationscale
    // int32_t to prevent overflow
    int32_t lDuration = duration;
    int32_t lCntr = m_Cntr;
    if (m_Period * lDuration < lCntr * DurationScale) {
      m_Gate = 0;
    }
  }

  m_ClockIn = clockIn;
  ++m_Cntr;

  return m_Gate;
}

class CVClock {
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

CVClock g_Clock(2, 3, A0);
CVClock g_Clock2(4, 5, A1);
int g_DebugCntr;
uint32_t g_DebugMillis;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("CVClock start...");

  g_Clock.Begin();
  g_Clock2.Begin();

  g_DebugCntr = 0;
  g_DebugMillis = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  // TODO alternate?
  g_Clock.Tick();
  g_Clock2.Tick();
  delay(1);

  // debug...
  ++g_DebugCntr;
  if (1000 < g_DebugCntr) {
    uint32_t time = millis();
    g_Clock.debugOut(time - g_DebugMillis);
    g_Clock2.debugOut(time - g_DebugMillis);
    g_DebugMillis = time;
    g_DebugCntr = 0;
  }
}
