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
