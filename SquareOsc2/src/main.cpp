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

  //TODO mix sq with noise depending on parameters
  oscOut = gater.gate ? squareOsc.oscOut : 0;
}

void printParameters()
{
  pc2.printf("\r\n");
  pc2.printf("GD %d\r\n", parameters.gateOnPeriod);
  pc2.printf("PP %d\r\n", parameters.pitchPeriod);
  pc2.printf("PD %d\r\n", parameters.pitchDecay);
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
    
    wait(0.1);//???

    ++debugCntr;
    if(10<debugCntr)
    {
      printParameters();
      debugCntr = 0;
    }
  }
}

