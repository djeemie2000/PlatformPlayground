#include <mbed.h>
#include "randomgen.h"
#include "noiseosc.h"
#include "phasor.h"
#include "parameters.h"
#include "noiseosc.h"
#include "squareosc.h"
#include "gater.h"
#include "smoother.h"
#include "mixer.h"
#include "fullosc.h"

Serial pc2(PA_9, PA_10); // tx, rx

DigitalOut led(LED1);
Ticker ticker;

DigitalIn debugIn(PA_8);

// 'freerunning' parameter from CV/pots
AnalogIn durationCV(PA_0);
AnalogIn pitchDecayCV(PA_1);
AnalogIn pitchPeriodCV(PA_2);
Smoother<uint32_t, 3> pitchPeriodSmoother(12);
AnalogIn noiseColorCV(PA_3);
AnalogIn modeCV(PA_4);

//Phasor<uint32_t, 8, 1> phasor;
//uint32_t phaseDelta;

// IOXP2(PB3, PA15, PA12, PA11) -> oscillator gate inputs
// IOXP1(PB15, PB14, PB13, PB12) -> oscillator outputs
// IOXP3(PB7, PB6, PB5, PB4) -> oscillator locked inputs

// oscillators common
Parameters freeRunningParams;
RandomGenUint16 randomGen;
// oscillators
FullOsc fullOsc1(PB_3, PB_15, PB_7);
FullOsc fullOsc2(PA_15, PB_14, PB_6);
FullOsc fullOsc3(PA_12, PB_13, PB_5);
FullOsc fullOsc4(PA_11, PB_12, PB_4);

void tick()
{
  randomGen.next();
  //phasor.tick(phaseDelta);

  fullOsc1.tick(randomGen.value);
  fullOsc2.tick(randomGen.value);
  fullOsc3.tick(randomGen.value);
  fullOsc4.tick(randomGen.value);
}

void printParameters(const Parameters& params)
{
  pc2.printf("\r\nM  %lu\r\n", params.mode);

  pc2.printf("GD %lu\r\n", params.gateOnPeriod);

  pc2.printf("PP %lu\r\n", params.pitchPeriod);
  pc2.printf("PD %lu\r\n", params.pitchDecay);

  pc2.printf("NC %lu\r\n", params.noiseColor);  
}

int main() {
  // put your setup code here, to run once:
  pc2.baud(115200);
  pc2.printf("SquareOsc...\r\n");

  int debugCntr = 0;

  //phaseDelta = 5;// scale = 8 -> 256
  randomGen.begin();

  // defaults
  freeRunningParams.pitchPeriod = 12;
  freeRunningParams.pitchDecay = 1;  

  fullOsc1.setParameters(freeRunningParams, true);
  fullOsc2.setParameters(freeRunningParams, true);
  fullOsc3.setParameters(freeRunningParams, true);
  fullOsc4.setParameters(freeRunningParams, true);

  us_timestamp_t period_us = 40;// 20KHz
  ticker.attach_us(tick, period_us);

  while(1) {
    //put your main code here, to run repeatedly:

    // continuously grab 'freerunning' parameters from CV/pots
    // check per FullOsc:
    // if 'osc locked' switch is off 
    //    copy 'freerunning' parameters into FullOsc parameters 
    // if 'osc locked' is on
    //   keep parameters for that FullOsc

    freeRunningParams.gateOnPeriod = durationCV.read_u16()>>1;
//    led  = fullOsc1.getGate();
    freeRunningParams.pitchDecay = pitchDecayCV.read_u16()>>8;//range[0, 256[
  //  led  = fullOsc.getGate();
    freeRunningParams.pitchPeriod = 2 + pitchPeriodSmoother.smooth( (pitchPeriodCV.read_u16()>>5) );// max pitchPeriod = 2048 ??
    //led  = fullOsc.getGate();
    freeRunningParams.noiseColor = (noiseColorCV.read_u16() >> 13); // [0,6] // = 1 + (noiseColorCV.read_u16() >> 13);//[1,8] 
    //-> problem: never 1 + 7???
    //led  = fullOsc.getGate();
    freeRunningParams.mode = modeCV.read_u16()>>14;//[0,3]
    //led  = fullOsc.getGate();

    // FullOsc will check its lock
    fullOsc1.setParameters(freeRunningParams);
    fullOsc2.setParameters(freeRunningParams);
    fullOsc3.setParameters(freeRunningParams);
    fullOsc4.setParameters(freeRunningParams);

    wait(0.04);//???

    ++debugCntr;
    if(25<debugCntr)
    {
      printParameters(freeRunningParams);
      debugCntr = 0;
    }
  }
}

