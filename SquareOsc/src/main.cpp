#include <Arduino.h>


static const int OscOutPin = 7;
static const int PitchInPin = A0;
static const int WaveInPin = A1;


void FastPin7On()
{
  PORTD = PORTD | (1<<7);
}

void FastPin7Off()
{
  PORTD = PORTD & ((1<<7) -1);
}

int oscOut;
int debugCntr;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("SquareOsc v0.1");

  pinMode(OscOutPin, OUTPUT);

  debugCntr = 0;
  oscOut = 1;
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t pitchPeriod = analogRead(PitchInPin);
  pitchPeriod = pitchPeriod << 2;// *32 => [0, 32000]
  if(pitchPeriod<5)
  {
    pitchPeriod = 5;
  }

  uint8_t wave = analogRead(WaveInPin) >> 2;// 1024 to 256

  int bitOut = bitRead(wave, oscOut);

  ++oscOut;
  if(8<= oscOut)
  {
    oscOut = 0;
  }

  if(bitOut)
  {
    FastPin7On();
  }
  else
  {
    FastPin7Off();
  }
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
  
  delayMicroseconds(pitchPeriod);//????????????

  ++debugCntr;
  if(debugCntr>5000)
  {
    Serial.print(pitchPeriod);
    Serial.print(' ');
    Serial.println(wave, HEX);
    debugCntr = 0;
  }
}
