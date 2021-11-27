#include <mbed.h>
#include "randomgen.h"
#include "noiseosc.h"
#include "phasor.h"
#include "parameters.h"
#include "noiseosc.h"
#include "squareosc.h"
#include "gater.h"


Serial pc2(PA_9, PA_10); // tx, rx

DigitalOut led(LED1);
Ticker ticker;

DigitalIn debugIn(PA_8);
DigitalIn gateIn(PB_14, PullUp);
DigitalOut oscOut(PB_15);
AnalogIn durationCV(PA_0);
AnalogIn pitchDecayCV(PA_1);
AnalogIn pitchPeriodCV(PA_2);
AnalogIn noiseColorCV(PA_3);
AnalogIn modeCV(PA_4);

Phasor<uint32_t, 8, 1> phasor;
uint32_t phaseDelta;

Parameters parameters;
RandomGenUint16 randomGen;
Gater gater;
SquareOsc squareOsc;
NoiseOsc noiseOsc;


void tick()
{
  randomGen.next();
  phasor.tick(phaseDelta);

  int extGate = gateIn;
  gater.tick(false, extGate, parameters.gateOnPeriod);

  bool reset = (gater.gateCntr == 0);
  squareOsc.tick(reset, parameters.pitchPeriod, parameters.pitchDecay);

  noiseOsc.tick(randomGen.value, parameters.noiseColor);

  //TODO mix sq with noise depending on parameters
  //oscOut = gater.gate ? squareOsc.oscOut : 0;

  if(0 == gater.gate)
  {
    // output off outside gate on period
      oscOut = 0;
  }
  else
  {    
    // gate is on
    if(parameters.mode==3)
    {
      // pure coloured noise
      oscOut = noiseOsc.oscOut;
    }
    else if(parameters.mode==2)
    {
      // mix square + coloured noise
      if(squareOsc.oscOut)
      {
        // square 1 and noise 1 => 1
        // square 1 and noise 0 => 0
        oscOut = noiseOsc.oscOut;
      }
      else
      {
        // square 0 and noise 1 => 0
        // square 0 and noise 0 => 1
         oscOut = (1-noiseOsc.oscOut);      
      }
      // ??? this is an xor : 1 if equal, 0 if not equal
    }
    else if(parameters.mode==1)
    {
      // mix square + coloured noise upon square on
      if(squareOsc.oscOut)
      {
        // square 1 and noise 1 => 1
        // square 1 and noise 0 => 0
        oscOut = noiseOsc.oscOut;
      }
      else
      {
        // square 0 and noise 1 => 0
        // square 0 and noise 0 => 0
        oscOut = 0;      
      }
      // this is an AND
    }
    else
    {
      // pure square
      oscOut = squareOsc.oscOut;
    }
  }
}

void printParameters()
{
  pc2.printf("\r\n%d\r\n", parameters.mode);

  pc2.printf("GD %d\r\n", parameters.gateOnPeriod);

  pc2.printf("PP %d\r\n", parameters.pitchPeriod);
  pc2.printf("PD %d\r\n", parameters.pitchDecay);

  pc2.printf("NC %d\r\n", parameters.noiseColor);  
}

int main() {

  
  // put your setup code here, to run once:
  pc2.baud(115200);
  pc2.printf("SquareOsc...\r\n");

  int debugCntr = 0;

  phaseDelta = 5;// scale = 8 -> 256
  randomGen.begin();


  //???
  parameters.pitchPeriod = 12;
  parameters.pitchDecay = 1;  

  us_timestamp_t period_us = 50;// 20KHz
  ticker.attach_us(tick, period_us);

  while(1) {
    //put your main code here, to run repeatedly:
    parameters.gateOnPeriod = durationCV.read_u16()>>1;
    led  = gater.gate;
    parameters.pitchDecay = pitchDecayCV.read_u16()>>8;//range[0, 256[
    led  = gater.gate;
    parameters.pitchPeriod = 2 + (pitchPeriodCV.read_u16()>>5);// max pitchPeriod = 2048 ??
    led  = gater.gate;
    parameters.noiseColor = 1 + (noiseColorCV.read_u16() >> 13);//[1,8]
    led  = gater.gate;
    parameters.mode = modeCV.read_u16()>>14;//[0,3]
    led  = gater.gate;
    
    wait(0.05);//???

    ++debugCntr;
    if(20<debugCntr)
    {
      printParameters();
      debugCntr = 0;
    }
  }
}

