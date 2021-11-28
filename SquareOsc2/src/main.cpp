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
RandomGenUint16 randomGen;

FullOsc fullOsc(PB_14, PB_15);
// Parameters parameters;
// DigitalIn gateIn(PB_14, PullUp);
// DigitalOut oscOut(PB_15);
// Gater gater;
// SquareOsc squareOsc;
// NoiseOsc noiseOsc;
// Mixer mixer;//stateless


void tick()
{
  randomGen.next();
  //phasor.tick(phaseDelta);

  fullOsc.tick(randomGen.value);
  // int extGate = gateIn;
  // gater.tick(false, extGate, parameters.gateOnPeriod);

  // bool reset = (gater.gateCntr == 0);
  // squareOsc.tick(reset, parameters.pitchPeriod, parameters.pitchDecay);

  // noiseOsc.tick(randomGen.value, parameters.noiseColor);

  // //TODO use LUT's? [0/1][0/1] 
  // //or index = squareOsc.oscOut | (noiseOsc.oscOut<<1) | (mode<<2); oscOut = LUT[index];

  // // mix sq with noise depending on parameters
  // if(0 == gater.gate)
  // {
  //   // output off outside gate on period
  //     oscOut = 0;
  // }
  // else
  // {    
  //   // gate is on
  //   oscOut = mixer.mix(squareOsc.oscOut, noiseOsc.oscOut, parameters.mode);

  //   // if(parameters.mode==3)
  //   // {
  //   //   // pure coloured noise
  //   //   oscOut = noiseOsc.oscOut;
  //   // }
  //   // else if(parameters.mode==2)
  //   // {
  //   //   // mix square + coloured noise
  //   //   if(squareOsc.oscOut)
  //   //   {
  //   //     // square 1 and noise 1 => 1
  //   //     // square 1 and noise 0 => 0
  //   //     oscOut = noiseOsc.oscOut;
  //   //   }
  //   //   else
  //   //   {
  //   //     // square 0 and noise 1 => 0
  //   //     // square 0 and noise 0 => 1
  //   //      oscOut = (1-noiseOsc.oscOut);      
  //   //   }
  //   //   // ??? this is an xor : 1 if equal, 0 if not equal
  //   // }
  //   // else if(parameters.mode==1)
  //   // {
  //   //   // mix square + coloured noise upon square on
  //   //   if(squareOsc.oscOut)
  //   //   {
  //   //     // square 1 and noise 1 => 1
  //   //     // square 1 and noise 0 => 0
  //   //     oscOut = noiseOsc.oscOut;
  //   //   }
  //   //   else
  //   //   {
  //   //     // square 0 and noise 1 => 0
  //   //     // square 0 and noise 0 => 0
  //   //     oscOut = 0;      
  //   //   }
  //   //   // this is an AND
  //   // }
  //   // else
  //   // {
  //   //   // pure square
  //   //   oscOut = squareOsc.oscOut;
  //   // }
  // }
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
  fullOsc.parameters.pitchPeriod = 12;
  fullOsc.parameters.pitchDecay = 1;  

  us_timestamp_t period_us = 50;// 20KHz
  ticker.attach_us(tick, period_us);

  while(1) {
    //put your main code here, to run repeatedly:

    // TODO continuously grab 'freerunning' parameters from CV/pots
    // TODO copy 'freerunning' parameters into FullOsc parameters if 'osc locked' switch is off
    // TODO if lock is on, keep parameters for that FullOsc

    fullOsc.parameters.gateOnPeriod = durationCV.read_u16()>>1;
    led  = fullOsc.gater.gate;
    fullOsc.parameters.pitchDecay = pitchDecayCV.read_u16()>>8;//range[0, 256[
    led  = fullOsc.gater.gate;
    fullOsc.parameters.pitchPeriod = 2 + pitchPeriodSmoother.smooth( (pitchPeriodCV.read_u16()>>5) );// max pitchPeriod = 2048 ??
    led  = fullOsc.gater.gate;
    fullOsc.parameters.noiseColor = 1 + (noiseColorCV.read_u16() >> 13);//[1,8]
    //-> problem: never 1 + 7???
    led  = fullOsc.gater.gate;
    fullOsc.parameters.mode = modeCV.read_u16()>>14;//[0,3]
    led  = fullOsc.gater.gate;

    
    wait(0.04);//???

    ++debugCntr;
    if(25<debugCntr)
    {
      printParameters(fullOsc.parameters);
      debugCntr = 0;
    }
  }
}

