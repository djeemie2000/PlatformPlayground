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

// common
Parameters freeRunningParams;
RandomGenUint16 randomGen;
FullOsc fullOsc(PB_14, PB_15);

void tick()
{
  randomGen.next();
  //phasor.tick(phaseDelta);

  fullOsc.tick(randomGen.value);
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

  //???
  freeRunningParams.pitchPeriod = 12;
  freeRunningParams.pitchDecay = 1;  

  us_timestamp_t period_us = 50;// 20KHz
  ticker.attach_us(tick, period_us);

  while(1) {
    //put your main code here, to run repeatedly:

    // continuously grab 'freerunning' parameters from CV/pots
    // TODO copy 'freerunning' parameters into FullOsc parameters if 'osc locked' switch is off
    // TODO if lock is on, keep parameters for that FullOsc

    freeRunningParams.gateOnPeriod = durationCV.read_u16()>>1;
    led  = fullOsc.getGate();
    freeRunningParams.pitchDecay = pitchDecayCV.read_u16()>>8;//range[0, 256[
    led  = fullOsc.getGate();
    freeRunningParams.pitchPeriod = 2 + pitchPeriodSmoother.smooth( (pitchPeriodCV.read_u16()>>5) );// max pitchPeriod = 2048 ??
    led  = fullOsc.getGate();
    freeRunningParams.noiseColor = 1 + (noiseColorCV.read_u16() >> 13);//[1,8]
    //-> problem: never 1 + 7???
    led  = fullOsc.getGate();
    freeRunningParams.mode = modeCV.read_u16()>>14;//[0,3]
    led  = fullOsc.getGate();

    fullOsc.setParameters(freeRunningParams);//TODO check lock

    
    wait(0.04);//???

    ++debugCntr;
    if(25<debugCntr)
    {
      printParameters(freeRunningParams);
      debugCntr = 0;
    }
  }
}

