#include <Arduino.h>

#include "fastpinio.h"
#include "parameters.h"
#include "gater.h"
#include "randomgen.h"
#include "squareosc.h"
#include "noiseosc.h"

static const int OscOutPin = 7;


int debugCntr;
unsigned long prevMillis;
Parameters parameters;
Gater gater;
RandomGenUint16 randomGen;
SquareOsc squareOsc;
NoiseOsc noiseOsc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("SquareOsc v0.1");

  pinMode(OscOutPin, OUTPUT);

  debugCntr = 0;
  prevMillis = 0;

  randomGen.begin();
}

void tick(const Parameters& params)
{
  gater.tick(params.gatePeriod, params.gateOnPeriod);
  bool reset = (gater.gateCntr == 0);

  randomGen.next();
  noiseOsc.tick(randomGen.value, params.noiseColor);
  squareOsc.tick(reset, params.pitchPeriod, params.pitchDecay); // + (params.pitchEnv>>9));// pitchenv >> 8 => [0, 128[

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
        // square 1
        FastPinSetPortD<7>(noiseOsc.oscOut);
      }
      else
      {
        // square 0
        FastPinSetPortD<7>(1-noiseOsc.oscOut);      
      }
    }
    else if(params.mode==1)
    {
      // mix square + coloured noise upon square on
      if(squareOsc.oscOut)
      {
        FastPinSetPortD<7>(noiseOsc.oscOut);
      }
      else
      {
        FastPinOffPortD<7>();      
      }
    }
    else
    {
      // pure square
      FastPinSetPortD<7>(squareOsc.oscOut);
    }
 }
}

void printDebug(const Parameters& params)
{
    ++debugCntr;
  if(gater.gateCntr == 0)// debugCntr>2000)
  {
    unsigned long currMillis=millis();
    unsigned long elapsed=currMillis-prevMillis;
    prevMillis = currMillis;

    Serial.print(elapsed);
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
}

void loop() {
  // put your main code here, to run repeatedly:
  printDebug(parameters);

  // => max pitch period = 2 + 1023 >> 2 = 2 + 255 = 257
  parameters.pitchPeriod = 2 + (analogRead(A0) >> 2);
  tick(parameters);

  parameters.pitchDecay = analogRead(A1)>>4;
  tick(parameters);

  parameters.gateOnPeriod = analogRead(A2) << 1;
  tick(parameters);

  uint16_t noiseColor = analogRead(A3);
  parameters.noiseColor = 1 + (noiseColor >> 7);//[1,8]
  tick(parameters);

  parameters.mode = analogRead(A4)>>8;//[0,3]
  tick(parameters);
}
