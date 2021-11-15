#include <Arduino.h>


static const int OscOutPin = 7;
static const int PitchInPin = A0;
static const int WaveInPin = A1;


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

template<int N>
class Osc
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

  void begin()
  {
      pinMode(N, OUTPUT);
  }

  void tick(uint16_t period)
  {
    phasePeriod = period;

    ++phaseCntr;
    if(phasePeriod<=phaseCntr)
    {
      phaseCntr =0;
      oscOut= 1- oscOut;
      if(oscOut)
      {
        FastPinOnPortD<N>();
      }
      else
      {
        FastPinOffPortD<N>();
      }      
    }
  }
};

//int oscOut;

int debugCntr;
Osc<7> osc1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("SquareOsc v0.1");

//  pinMode(OscOutPin, OUTPUT);

  debugCntr = 0;
  //oscOut = 1;

  osc1.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  int pitchPeriod = analogRead(PitchInPin) >> 2;
  //pitchPeriod = pitchPeriod >> 2;//  [0, 64[
  if(pitchPeriod<2)
  {
    pitchPeriod = 2;
  }

  osc1.tick(pitchPeriod);

  // uint8_t wave = analogRead(WaveInPin) >> 2;// 1024 to 256

  // int bitOut = bitRead(wave, oscOut);

  // ++oscOut;
  // if(8<= oscOut)
  // {
  //   oscOut = 0;
  // }

  // if(bitOut)
  // {
  //   FastPinOnPortD<7>();
  // }
  // else
  // {
  //   FastPinOffPortD<7>();
  // }
  // if(oscOut)
  // {
  //   FastPin7On();
  //   oscOut = 0;
  // }
  // else
  // {
  //   FastPin7Off();
  //   oscOut = 1;
  // }
  
//  delayMicroseconds(1);//????????????

  ++debugCntr;
  if(debugCntr>2000)
  {
    Serial.print(osc1.phasePeriod);
    Serial.print(' ');
    Serial.println(osc1.phaseCntr);
    debugCntr = 0;
  }
}
