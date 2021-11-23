#include <Arduino.h>

#include "fastpinio.h"
#include "parameters.h"
#include "gater.h"
#include "randomgen.h"
#include "squareosc.h"
#include "noiseosc.h"
#include "triggerin.h"

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
  // locked input pins:
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  
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

  TriggerInPortD<2> trigger0;// PD2 pin 2
  TriggerInPortD<3> trigger1;
  TriggerInPortD<4> trigger2;

  TriggerInPortD<5> locked0;
  TriggerInPortD<6> locked1;
  TriggerInPortD<8> locked2;


//  int prevExtGate = -1;
  int activeOsc = 0;
  while(true)
  {
    ++debugCntr;

    trigger0.tick();
    trigger1.tick();
    trigger2.tick();

    locked0.tick();
    locked1.tick();
    locked2.tick();

    // only change actoveOsc upon triggered
    bool retrigger = false;
    if(trigger0.triggered)
    {
      activeOsc = 0;
      retrigger = true;
    }
    else if(trigger1.triggered)
    {
      activeOsc = 1;
      retrigger = true;
    }
    else if(trigger2.triggered)
    {
      activeOsc = 2;
      retrigger = true;
    }

    int activeGate = 0;
    int activeReleased = 0;
    int activeLocked = 0;
    if(activeOsc == 0)
    {
      activeGate = trigger0.currGate;
      activeReleased = trigger0.released;
      activeLocked = locked0.currGate;
    }
    else if(activeOsc == 1)
    {
      activeGate = trigger1.currGate;
      activeReleased = trigger1.released;
      activeLocked = locked1.currGate;
    }
    else if(activeOsc == 2)
    {
      activeGate = trigger2.currGate;
      activeReleased = trigger2.released;
      activeLocked = locked2.currGate;
    }

    if(activeReleased)
    {
      // keep activeOsc
      printDebug(activeOsc, parameters[activeOsc]);// print once when ext gate goes to off
    }

    if(retrigger && !activeLocked)
    {
      //Serial.println('T');

      // read/grab all parameters, then tick using these parameters  
      GrabParameters(parameters[activeOsc]);
    }


    tick(retrigger, activeGate, parameters[activeOsc]);
  }


}
