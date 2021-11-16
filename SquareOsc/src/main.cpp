#include <Arduino.h>

#include "gater.h"
#include "random.h"
#include "squareosc.h"
#include "noiseosc.h"

static const int OscOutPin = 7;

template<int N>
void FastPinOnPortD()
{
  PORTD |= (1<<N);
}

template<int N>
void FastPinOffPortD()
{
  PORTD &= ~(1<<N);
}



struct Parameters
{

  uint16_t gatePeriod;

  uint16_t mode;//SQ, mix, Noise

  uint16_t pitchPeriod;
  uint16_t pitchDecay;
  uint16_t noiseColor;
  uint16_t gateOnPeriod;

  Parameters()
   : gatePeriod(2048)
   , mode(0)
   , pitchPeriod(64)
   , pitchDecay(0)
   , noiseColor(0)
   , gateOnPeriod(1024)
  {}

};


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

//  squareOsc.begin();
  randomGen.begin();
}

void tick()
{
  gater.tick(parameters.gatePeriod, parameters.gateOnPeriod);
  bool reset = (gater.gateCntr == 0);

  randomGen.next();
  noiseOsc.tick(randomGen.value, parameters.noiseColor);
  squareOsc.tick(reset, parameters.pitchPeriod, parameters.pitchDecay); // + (parameters.pitchEnv>>9));// pitchenv >> 8 => [0, 128[

  if(0 == gater.gate)
  {
    // output off outside gate on period
      FastPinOffPortD<7>();
  }
  else
  {    
    // gate is on
    if(parameters.mode>=8)
    {
      // pure uncolored noise
      if(noiseOsc.oscOut) //(randomGen.value & 1))
      {
        FastPinOnPortD<7>();
      }
      else
      {
        FastPinOffPortD<7>();      
      }      
    }
    else if(parameters.mode>0)
    {
      // mix square + coloured noise
      //uint16_t mask = 0xFF>>parameters.noiseColor;//[0,7]
      //if( (randomGen.value & mask) && squareOsc.oscOut)
      if(noiseOsc.oscOut && squareOsc.oscOut)
      {
        FastPinOnPortD<7>();
      }
      else
      {
        FastPinOffPortD<7>();      
      }
    }
    else
    {
      // pure square
      if(squareOsc.oscOut)
      {
        FastPinOnPortD<7>();
      }
      else
      {
        FastPinOffPortD<7>();      
      }
    }
 }
}

void printDebug()
{
    ++debugCntr;
  if(gater.gateCntr == 0)// debugCntr>2000)
  {
    unsigned long currMillis=millis();
    unsigned long elapsed=currMillis-prevMillis;
    prevMillis = currMillis;

    Serial.print(elapsed);
    Serial.print(':');
    Serial.print(parameters.mode);
    Serial.print(' ');
    Serial.print(parameters.gateOnPeriod);
    Serial.print(' ');
    Serial.print(parameters.gatePeriod);
    Serial.print(' ');
    Serial.print(parameters.pitchDecay);
    Serial.print(' ');
    Serial.println(parameters.pitchPeriod);
    debugCntr = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  printDebug();

  // => max pitch period = 2 + 1023 >> 2 = 2 + 255 = 257
  parameters.pitchPeriod = 2 + (analogRead(A0) >> 2);
  tick();

  parameters.pitchDecay = analogRead(A1)>>4;
  tick();

  parameters.gateOnPeriod = analogRead(A2) << 1;
  tick();

  uint16_t mode = analogRead(A3);
  parameters.mode = mode>1000 ? 8 : mode >> 7;//[0,7]
  parameters.noiseColor = parameters.mode;
  tick();
}
