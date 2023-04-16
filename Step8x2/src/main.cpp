#include <Arduino.h>

#include "digitalin.h"

struct Step8App
{
  static const int MaxNumSteps = 8;

  DigitalIn m_ClockIn;
  DigitalIn m_ResetIn;
  int m_ClockOutPin;
  int m_ResetOutPin;
  int m_APin;
  int m_BPin;
  int m_CPin;

  // parameters
  int m_Divide;   // 1, 2, 4, 8, 16, 3, 6, 12
  int m_Length;   // 1, 2, 3, 4, 5, 6, 7, 8
  int m_StepSize; // 0, 1, 2, 3, 4, 5, 6, 7

  // state
  bool m_DoReset;
  int m_Step;
  // clock divider state
  int m_Clock;
  int m_PrevClock;
  int m_ClockCounter;

  Step8App()
      : m_Divide(1), m_Length(MaxNumSteps), m_StepSize(1), m_DoReset(false), m_Step(0), m_Clock(0), m_PrevClock(0), m_ClockCounter(0)
  {
  }

  void Begin(int clockInPin,
             int resetInPin,
             int clockOutPin,
             int resetOutPin,
             int aPin,
             int bPin,
             int cPin)
  {
    m_ClockIn.begin(clockInPin, false);
    m_ResetIn.begin(resetInPin, false);
    m_ClockOutPin = clockOutPin;
    pinMode(m_ClockOutPin, OUTPUT);
    m_ResetOutPin = resetOutPin;
    pinMode(m_ResetOutPin, OUTPUT);
    m_APin = aPin;
    pinMode(m_APin, OUTPUT);
    m_BPin = bPin;
    pinMode(m_BPin, OUTPUT);
    m_CPin = cPin;
    pinMode(m_CPin, OUTPUT);

    m_Step = 0;
  }

  void Update()
  {
    // read reset rising => raise reset flag
    // read clock rising => advance
    m_ResetIn.Read();
    if (m_ResetIn.IsRising())
    {
      m_DoReset = true;
    }

    // read CV for divide, length, stepsize
    // TODO analogRead

    // TODO clock divider!
    m_ClockIn.Read();
    UpdateClock();


    if (m_Clock && !m_PrevClock)
    {
      // divided clock is rising => advance step
      AdvanceStep();
    }

    // set step
    WriteStep(m_Step);
    // set reset out
    WriteReset();
    // set clock out
    digitalWrite(m_ClockOutPin, m_Clock);
  }

  void UpdateClock()
  {
    m_PrevClock = m_Clock;

    if (m_DoReset && m_ClockIn.IsRising())
    {
      m_Clock = 1;
      m_ClockCounter = 0;
    }
    else if (m_ClockIn.IsRising() || m_ClockIn.IsFalling())
    {
      ++m_ClockCounter;
      if (m_Divide <= m_ClockCounter)
      {
        m_ClockCounter = 0;
        // toggle clock
        m_Clock = 1 - m_Clock;
      }
    }

  }

  void AdvanceStep()
  {
    if (m_DoReset)
    {
      m_DoReset = false;
      m_Step = 0;
    }
    else
    {
      // use parameters (length and step size) to advance the step
      // no good alternative to avoid division that works in all cases
      // assumes length>=1
      // step size can be zero
      m_Step = (m_Step + m_StepSize) % m_Length;
    }
  }

  void WriteStep(int step)
  {
    digitalWrite(m_APin, bitRead(step, 0));
    digitalWrite(m_BPin, bitRead(step, 1));
    digitalWrite(m_CPin, bitRead(step, 2));
  }

  void WriteReset()
  {
    if (0 == m_Step)
    {
      // use divided clock
      digitalWrite(m_ResetOutPin, m_Clock);
    }
    else
    {
      digitalWrite(m_ResetOutPin, LOW);
    }
  }

  void TestOutputs()
  {
    Serial.println("TestOutputs");

    int intervalMs = 200;
    for (uint8_t step = 0; step < MaxNumSteps; ++step)
    {
      WriteStep(step);

      digitalWrite(m_ClockOutPin, HIGH);
      if (0 == step)
      {
        digitalWrite(m_ResetOutPin, HIGH);
      }
      delay(intervalMs);
      digitalWrite(m_ClockOutPin, LOW);
      digitalWrite(m_ResetOutPin, LOW);
      delay(intervalMs);
    }
  }

  void TestInputs()
  {
    Serial.println("TestInputs");

    int intervalMs = 2;
    for (int repeats = 0; repeats < 4000; ++repeats)
    {
      m_ClockIn.Read();
      digitalWrite(m_ClockOutPin, m_ClockIn.Get());

      m_ResetIn.Read();
      digitalWrite(m_ResetOutPin, m_ResetIn.Get());

      if (m_ClockIn.IsRising())
      {
        Serial.println("CR");
      }
      if (m_ClockIn.IsFalling())
      {
        Serial.println("CF");
      }
      if (m_ResetIn.IsRising())
      {
        Serial.println("RR");
      }
      if (m_ResetIn.IsFalling())
      {
        Serial.println("RF");
      }

      delay(intervalMs);
    }
  }

  void TestAnalogIn()
  {
    Serial.println("TestAnalogIn");
    // TODO read + println
  }
};

Step8App app1;
Step8App app2;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Step8x2 V0.1...");

  app1.Begin(2, 3, 4, 5, 6, 7, 8);
  app2.Begin(9, 10, 11, 12, 13, 14, 15);

  delay(1000);
}

void loop()
{
  // put your main code here, to run repeatedly:
  app1.TestOutputs();
  app2.TestOutputs();

  app1.TestInputs();
  app2.TestInputs();
}
