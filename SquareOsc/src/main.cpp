#include <Arduino.h>


static const int OscOutPin = 7;
static const int PitchInPin = A0;
static const int PitchInPin2 = A1;


// void FastPin7On()
// {
//   PORTD |= (1<<7);
// }

// void FastPin7Off()
// {
//   PORTD &= ~(1<<7);
// }

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
public :
  uint16_t pitchPeriod1;
  uint16_t pitchPeriod2;
  uint16_t gatePeriod;
  uint16_t gateOnPeriod;

  uint16_t gateCntr;

  Parameters()
   : pitchPeriod1(64)
   , pitchPeriod2(64)
   , gatePeriod(4096)
   , gateOnPeriod(512)
   , gateCntr(0)
  {}

  void tick()
  {
    ++gateCntr;
    if(gatePeriod<=gateCntr)
    {
      gateCntr = 0;
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

//int oscOut;

int debugCntr;
unsigned long prevMillis;
Parameters parameters;
Osc osc1;
Osc osc2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("SquareOsc v0.1");

  pinMode(OscOutPin, OUTPUT);

  debugCntr = 0;
  prevMillis = 0;

//  osc1.begin();
}

void tick()
{
  parameters.tick();
  osc1.tick(parameters.pitchPeriod1);
  osc2.tick(parameters.pitchPeriod2);

  //if(osc1.phaseCntr == 0 || osc2.phaseCntr == 0)
  if(parameters.gateCntr >= parameters.gateOnPeriod)
  {
      FastPinOffPortD<7>();
  }
  else
  {    
    // xorOut 
    if(osc1.oscOut != osc2.oscOut)
    {
      FastPinOnPortD<7>();
    }
    else
    {
      FastPinOffPortD<7>();
    }   
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  parameters.pitchPeriod1 = 2 + (analogRead(PitchInPin) >> 2);
  tick();

  parameters.pitchPeriod2 = 2 + (analogRead(PitchInPin2) >> 2);
  tick();

  
  ++debugCntr;
  if(parameters.gateCntr == 0)// debugCntr>2000)
  {
    unsigned long currMillis=millis();
    unsigned long elapsed=currMillis-prevMillis;
    prevMillis = currMillis;

    Serial.print(elapsed);
    Serial.print(':');
    Serial.print(parameters.gateOnPeriod);
    Serial.print(' ');
    Serial.print(parameters.gatePeriod);
    Serial.print(' ');
    Serial.print(osc1.phasePeriod);
    Serial.print(' ');
    Serial.println(osc2.phasePeriod);
    debugCntr = 0;
  }
}
