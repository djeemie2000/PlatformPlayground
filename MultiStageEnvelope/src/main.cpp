#include "GateIn.h"
#include "MBedUnit.h"
#include "Mcp4822VoltageOut.h"
#include <mbed.h>

class Stage {
public:
  Stage();

  bool Update(int &value);

  int m_Target;
  int m_Slope;
};

Stage::Stage() : m_Target(0), m_Slope(0) {}

bool Stage::Update(int &value) {
  // calculate new value
  int newValue = value;
  if (value < m_Target - m_Slope) {
    newValue = value + m_Slope;
  } else if (m_Target + m_Slope < value) {
    newValue = value - m_Slope;
  }

  // advance when crossing target
  bool advance = (newValue <= m_Target) == (m_Target < value);

  // update value
  value = newValue;

  // ...
  return advance;
}

class TimeBasedEnvelope {
public:
  static const int SkipDuration = 0;
  static const int HoldDuration = -1;

  TimeBasedEnvelope()
      : m_EnvelopeValue(0.0f), m_ElapsedTime(0), m_CurrentStage(1),
        m_PreviousStage(0) {
    for (int idx = 0; idx < NumStages; ++idx) {
      m_Level[idx] = idx % 2 ? 1.0f : 0.0f;
      m_Duration[idx] = 1024;
    }
  }

  // TODO update level/duration from CV input
  void SetLevel(float level, int stage) { m_Level[stage] = level; }

  // TODO update level/duration from CV input
  void SetDuration(int duration, int stage) { m_Duration[stage] = duration; }

  void Tick(int gate) {
    // TODO duration based on gate and current stage

    ++m_ElapsedTime;

    int remainingTime = m_Duration[m_CurrentStage] - m_ElapsedTime;
    if (remainingTime <= 0) {
      //  advance stage
      m_ElapsedTime = 0;
      m_PreviousStage = m_CurrentStage;
      ++m_CurrentStage;
      if (NumStages <= m_CurrentStage) {
        m_CurrentStage = 0;
      }
      remainingTime = m_Duration[m_CurrentStage];
    }

    if (SkipDuration == m_Duration[m_CurrentStage]) {
      // immediate/skip
      // => envelope value unaffected, handle next stage in next Tick() call
    } else if (HoldDuration == m_Duration[m_CurrentStage]) {
      // HOLD
      // => envelope value equal to previous stage level
      // (cfr sustain in ADSR)
      m_EnvelopeValue = m_Level[m_PreviousStage];
    } else {
      // increase / decrease envelope based on line towards current stage level
      float slope = (m_Level[m_CurrentStage] - m_EnvelopeValue) / remainingTime;
      m_EnvelopeValue += slope;
      //  crop envelope value to [0,1]
      if (m_EnvelopeValue < 0) {
        m_EnvelopeValue = 0.0f;
      } else if (1 < m_EnvelopeValue) {
        m_EnvelopeValue = 1.0f;
      }
    }
  }

private:
  static const int NumStages = 4;

  float m_Level[NumStages];  // (target) level [stage]
  int m_Duration[NumStages]; // duration[0] = duration between stage 0  and 1,
                             // etc...
  float m_EnvelopeValue;
  int m_ElapsedTime;
  int m_CurrentStage;
  int m_PreviousStage;
};

int main() {

  // put your setup code here, to run once:
  DigitalOut ledOut(PC_13);        // builtin led as gate indicator
  Serial debugSerial(PA_9, PA_10); // tx, rx
  debugSerial.baud(115200);

  // short pause before running
  wait_ms(2000);

  GateIn gateIn(PB_13);                  // external clock
  SPI spiPort(PA_7, PA_6, PA_5, NC);     // TODO chip select?
  MCP4x22 mcp(&spiPort, PA_8);           //????
  Mcp4822VoltageOut voltageOut(&mcp, 0); // channel A for envelope
  // TODO channel B for stepped?

  const int NumStages = 4;
  Stage gateOnStages[NumStages];
  Stage gateOffStages[NumStages];
  int currentStage = 0;

  // init envelope, assume gate off
  int envelopeValue = gateOffStages[0].m_Target;

  // testMain(debugSerial);//TODO debug

  // start running
  debugSerial.printf("\r\n-\r\n-\r\nMulti stage envelope...\r\n-\r\n-\r\n");
  debugSerial.printf("version 0.1\r\n");
  wait_ms(1000);

  while (1) {
    // put your main code here, to run repeatedly:

    Timer timer;
    int counter = 0;

    timer.reset();
    timer.start();

    for (int repeat = 0; repeat < 500; ++repeat) {
      // TODO read target / slope of current stage?
      // TODO alternate reading CV's??

      gateIn.Read();

      // check which stage applies
      // update with this state
      bool advance = false;
      if (gateIn.Get()) {
        advance = gateOnStages[currentStage].Update(envelopeValue);
      } else {
        advance = gateOffStages[currentStage].Update(envelopeValue);
      }
      // advance if needed
      if (advance) {
        currentStage++;
        if (NumStages <= currentStage) {
          currentStage = 0;
        }
      }

      // output envelope value

      //
      wait_ms(1);
    }

    timer.stop();
    debugSerial.printf("stage %d gate %d \r\n", currentStage, gateIn.Get());
    debugSerial.printf("%d : %d ms\r\n", counter++, timer.read_ms());
  }
}
