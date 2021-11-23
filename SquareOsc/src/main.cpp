#include <Arduino.h>

#include "fastpinio.h"
#include "parameters.h"
#include "gater.h"
#include "randomgen.h"
#include "squareosc.h"
#include "noiseosc.h"

static const int OscOutPin = 7;


uint16_t debugCntr;
unsigned long prevMillis;
static const int NumSlots = 3;
Parameters parameters[NumSlots];
Gater gater;
RandomGenUint16 randomGen;
SquareOsc squareOsc;
NoiseOsc noiseOsc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("SquareOsc v0.1");

  pinMode(OscOutPin, OUTPUT);
  // trigger in pins:
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  debugCntr = 0;
  prevMillis = 0;

  randomGen.begin();
}

// TODO template<int OutPin>
void tick(bool retrigger, int extGate, const Parameters& params)
{
  randomGen.next();
  // reset/retrigger gater upon change of extGate/activeOsc
  gater.tick(retrigger, extGate, params.gateOnPeriod);
  bool reset = (gater.gateCntr == 0);
  noiseOsc.tick(randomGen.value, params.noiseColor);
  squareOsc.tick(reset, params.pitchPeriod, params.pitchDecay); // + (params.pitchEnv>>9));// pitchenv >> 8 => [0, 128[

    // combine 
  if(0 == gater.gate)
  {
    // output off outside gate on period
      FastPinOffPortD<7>();
  }
  else
  {    
    // gate is on
    if(params.mode==3)
    {
      // pure coloured noise
      FastPinSetPortD<7>(noiseOsc.oscOut);
    }
    else if(params.mode==2)
    {
      // mix square + coloured noise
      if(squareOsc.oscOut)
      {
        // square 1 and noise 1 => 1
        // square 1 and noise 0 => 0
        FastPinSetPortD<7>(noiseOsc.oscOut);
      }
      else
      {
        // square 0 and noise 1 => 0
        // square 0 and noise 0 => 1
        FastPinSetPortD<7>(1-noiseOsc.oscOut);      
      }
      // ??? this is an xor : 1 if equal, 0 if not equal
    }
    else if(params.mode==1)
    {
      // mix square + coloured noise upon square on
      if(squareOsc.oscOut)
      {
        // square 1 and noise 1 => 1
        // square 1 and noise 0 => 0
        FastPinSetPortD<7>(noiseOsc.oscOut);
      }
      else
      {
        // square 0 and noise 1 => 0
        // square 0 and noise 0 => 0
        FastPinOffPortD<7>();      
      }
      // this is an AND
    }
    else
    {
      // pure square
      FastPinSetPortD<7>(squareOsc.oscOut);
    }
 }
}

void printDebug(int activeOsc, const Parameters& params)
{
// 
    unsigned long currMillis=millis();
    unsigned long elapsed=currMillis-prevMillis;
    prevMillis = currMillis;

    Serial.print(debugCntr);
    Serial.print(' ');
    Serial.print(elapsed);
    Serial.print(' ');
    Serial.print('O');
    Serial.print(activeOsc);
    Serial.print(' ');
    Serial.print('M');
    Serial.print(params.mode);
    Serial.print(' ');
    Serial.print('T');
    Serial.print(params.gateOnPeriod);
    Serial.print(' ');
    Serial.print('G');
    Serial.print(params.gatePeriod);
    Serial.print(' ');
    Serial.print('D');
    Serial.print(params.pitchDecay);
    Serial.print(' ');
    Serial.print('P');
    Serial.print(params.pitchPeriod);
    Serial.print(' ');
    Serial.print('C');
    Serial.println(params.noiseColor);
    debugCntr = 0;
}

void GrabParameters(Parameters& params)
{
    params.pitchPeriod = 2 + analogRead(A0);// max pitchPeriod = 2048 ??
    params.pitchDecay = analogRead(A1);
    params.pitchDecay = (params.pitchDecay>>3);//range[0, 128]
    params.gateOnPeriod = ((uint32_t)analogRead(A2)) << 6; // range [0, 1<<16]
    params.noiseColor = 1 + (analogRead(A3) >> 7);//[1,8]
    params.mode = analogRead(A4)>>8;//[0,3]
}

void loop() {
  // put your main code here, to run repeatedly:
  int prevExtGate = -1;
  int activeOsc = 0;
  while(true)
  {
    ++debugCntr;

    int extGate = -1;
    if(FastPinGetPortD<2>())
    {
      extGate = 0;// PD2 pin 2
    }
    else if(FastPinGetPortD<3>())
    {
      extGate = 1;
    }
    else if(FastPinGetPortD<4>())
    {
      extGate = 2;
    }
    bool triggered = (extGate != prevExtGate) && (0<=extGate);
    bool released =  (extGate != prevExtGate) && (extGate<0);
    prevExtGate = extGate;

    if(released)
    {
      // keep activeOsc
      printDebug(activeOsc, parameters[activeOsc]);// print once when ext gate goes to off
    }

    if(triggered)
    {
      // only change actoveOsc upon triggered
      activeOsc = extGate;

      //Serial.println('T');

      // read/grab all parameters, then tick using these parameters  
      GrabParameters(parameters[activeOsc]);
      // parameters[activeOsc].pitchPeriod = 2 + analogRead(A0);// max pitchPeriod = 2048 ??
      // parameters[activeOsc].pitchDecay = analogRead(A1);
      // parameters[activeOsc].pitchDecay = (parameters[activeOsc].pitchDecay>>3);//range[0, 128]
      // parameters[activeOsc].gateOnPeriod = ((uint32_t)analogRead(A2)) << 6; // range [0, 1<<16]
      // parameters[activeOsc].noiseColor = 1 + (analogRead(A3) >> 7);//[1,8]
      // parameters[activeOsc].mode = analogRead(A4)>>8;//[0,3]
    }

    tick(triggered, 0<=extGate?1:0, parameters[activeOsc]);
  }


}
