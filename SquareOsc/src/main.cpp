#include <Arduino.h>


static const int OscOutPin = 7;
//static const int PitchInPin = A0;
//static const int PitchInPin2 = A1;

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


struct RandomUint16
{
  RandomUint16() : value(0xF0F0)
  {}

  void begin(uint16_t seed = 0xFE89)
  {
    value = seed;
  }

  void next()
  {
    value = (value >> 0x01U) ^ (-(value & 0x01U) & 0xB400U);
  }

  uint16_t value;
};

struct Parameters
{
public :
  uint16_t mode;

  uint16_t pitchPeriod1;
  uint16_t pitchDecay;
  uint16_t gatePeriod;
  uint16_t gateOnPeriod;

  uint16_t gateCntr;
  uint16_t pitchEnv;

  Parameters()
   : mode(0)
   , pitchPeriod1(64)
   , pitchDecay(0)
   , gatePeriod(2048)
   , gateOnPeriod(1024)
   , gateCntr(0)
   , pitchEnv(0)
  {}

  void tick()
  {
    ++gateCntr;
    pitchEnv += pitchDecay;
    if(gatePeriod<=gateCntr)
    {
      gateCntr = 0;
      pitchEnv = 0;
    }
  }
};

struct Osc
{
public:
  uint16_t phaseCntr;
  uint16_t phasePeriod;

  int oscOut;

  Osc()
   : phaseCntr(0)
   , phasePeriod(2048)
   , oscOut(0)
  {    
  }

  void tick(uint16_t period)
  {
    phasePeriod = period;

    ++phaseCntr;
    if(phasePeriod<=phaseCntr)
    {
      phaseCntr =0;
      oscOut= 1- oscOut;   
    }
  }
};

int debugCntr;
unsigned long prevMillis;
Parameters parameters;
Osc squareOsc;
//Osc osc2;
RandomUint16 randomOsc;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("SquareOsc v0.1");

  pinMode(OscOutPin, OUTPUT);

  debugCntr = 0;
  prevMillis = 0;

//  squareOsc.begin();
  randomOsc.begin();
}

void tick()
{
  parameters.tick();
  randomOsc.next();
  squareOsc.tick(parameters.pitchPeriod1 + (parameters.pitchEnv>>9));
  //osc2.tick(parameters.pitchPeriod2);

  //if(squareOsc.phaseCntr == 0 || osc2.phaseCntr == 0)
  if(parameters.gateCntr >= parameters.gateOnPeriod)
  {
      FastPinOffPortD<7>();
  }
  else
  {    
    // XOR squareOsc osc2 
    // AND osc random -> OK
    // OR  SQ random -> ok
    // TODO slicer xor vs random
    if(parameters.mode>=8)
    {
      if( (randomOsc.value & 1))
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
      uint16_t mask = 0xFF>>parameters.mode;
      if( (randomOsc.value & mask) && squareOsc.oscOut)
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

    // if(osc1.oscOut != osc2.oscOut)
    // {
    //   FastPinOnPortD<7>();
    // }
    // else
    // {
    //   FastPinOffPortD<7>();
    // }   
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  parameters.pitchPeriod1 = 2 + (analogRead(A0) >> 2);
  tick();

  parameters.pitchDecay = analogRead(A1)>>4;
  tick();

  parameters.gateOnPeriod = analogRead(A2) << 1;
  tick();

  uint16_t mode = analogRead(A3);
  parameters.mode = mode>1000 ? 8 : mode >> 7;//[0,7]
  tick();
  
  ++debugCntr;
  if(parameters.gateCntr == 0)// debugCntr>2000)
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
    Serial.println(squareOsc.phasePeriod);
    debugCntr = 0;
  }
}
