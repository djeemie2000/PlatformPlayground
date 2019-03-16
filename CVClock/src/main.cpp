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
    if (m_Period * duration < m_Cntr * DurationScale) {
      m_Gate = 0;
    }
  }

  m_ClockIn = clockIn;
  ++m_Cntr;

  return m_Gate;
}

const int clockInPin = 2;
const int gateOutPin = 3;
const int durationCVPin = A0;
CVClockState g_state;
int g_DebugCntr;
uint32_t g_DebugMillis;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("CVClock start...");

  g_DebugCntr = 0;
  g_DebugMillis = 0;

  pinMode(clockInPin, INPUT);
  pinMode(gateOutPin, OUTPUT);
  pinMode(durationCVPin, INPUT);
}

void debugOut(uint32_t elapsed, const CVClockState &state, int duration) {
  Serial.print(elapsed);
  Serial.print(" : ");
  Serial.print(state.Period());
  Serial.print(" ");
  Serial.println(duration);
}

void loop() {
  // put your main code here, to run repeatedly:
  int clockIn = digitalRead(clockInPin); // TODO fast digital read!
  int duration = analogRead(durationCVPin);
  int gateOut = g_state.Tick(clockIn, duration);
  digitalWrite(gateOutPin, gateOut);

  delay(1);

  // debug...
  ++g_DebugCntr;
  if (1000 < g_DebugCntr) {
    uint32_t time = millis();
    debugOut(time - g_DebugMillis, g_state, duration);
    g_DebugMillis = time;
    g_DebugCntr = 0;
  }
}