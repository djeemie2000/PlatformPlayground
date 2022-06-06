#include <Arduino.h>
#include <Wire.h>

#include "ScanI2C.h"
#include "mcp4728.h"
#include "midinoteparser.h"
#include "testmidinoteparser.h"

// outputs: 2x gate 2x pitch 2x velocity
const int gatePin0 = 4;
const int gatePin1 = 5;
MCP4728Dac Dac1;
MidiNoteParser midiNoteParser;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200); // TODO midi Rx/Tx

  Serial.println("Midi2CV poc ...");

  // PWM out -> RC filter 104 47k -> buffer opamp
  // setupNormalPwm();

  pinMode(gatePin0, OUTPUT);
  pinMode(gatePin1, OUTPUT);
  Dac1.Begin(MCP4728Dac::MCP4728_I2CADDR_DEFAULT);
  // pitch channels : internal ref + gain x2 => 0-4.096 V
  // velocity channels : VDD (5V) ref + gain x1 => 0-5V
  Dac1.ConfigureChannel(0, MCP4728Dac::vref::MCP4728_VREF_INTERNAL, MCP4728Dac::gain::MCP4728_GAIN_2X);
  Dac1.ConfigureChannel(1, MCP4728Dac::vref::MCP4728_VREF_VDD, MCP4728Dac::gain::MCP4728_GAIN_1X);
  Dac1.ConfigureChannel(2, MCP4728Dac::vref::MCP4728_VREF_INTERNAL, MCP4728Dac::gain::MCP4728_GAIN_2X);
  Dac1.ConfigureChannel(3, MCP4728Dac::vref::MCP4728_VREF_VDD, MCP4728Dac::gain::MCP4728_GAIN_1X);
}

void TestDac()
{
  // test Dac
  const int delayPeriod = 400;
  for (int ch = 0; ch < MCP4728Dac::NumChannels; ++ch)
  {
    // 12 bits value
    for (int value = 0; value < 4096; value += 500)
    {
      Dac1.SetValue(ch, value);
      Dac1.Update();

      Serial.print("ch ");
      Serial.print(ch);
      Serial.print(" : ");
      Serial.println(value);

      delay(delayPeriod);
    }
  }
}

void NoteOn(int midiNote, int velocity, int channel)
{
  Serial.print("ch ");
  Serial.print(channel);
  Serial.print(" : note on ");
  Serial.print(midiNote);
  Serial.print(" vel ");
  Serial.println(velocity);

  if (channel == 0 || channel == 1)
  {
    // max note = 4096 / 1000 * 12 = 49
    int midiNoteInternal = midiNote <= 49 ? midiNote : 49;
    int32_t value1 = midiNoteInternal * 1000l / 12;
    Dac1.SetValue(channel * 2, value1);

    // velocity [0,127] 7 bits -> scale up to 12 bits
    int value2 = velocity << 5;
    Dac1.SetValue(channel * 2 + 1, value2);

    Dac1.Update();

    // gate on
    if (channel == 0)
    {
      digitalWrite(gatePin0, HIGH);
    }
    else if (channel == 1)
    {
      digitalWrite(gatePin1, HIGH);
    }
  }
}

void NoteOff(int channel)
{
  Serial.print("ch ");
  Serial.print(channel);
  Serial.println(" : note off ");

  // keep pitch cv out
  // keep velocity cv out
  // gate off
  if (channel == 0)
  {
    digitalWrite(gatePin0, LOW);
  }
  else if (channel == 1)
  {
    digitalWrite(gatePin1, LOW);
  }
}

void TestPlayScale()
{
  int scale[] = {0, 2, 4, 5, 7, 9, 11, 12};
  int velocity = 0;
  for (int octave = 0; octave < 4; ++octave)
  {
    for (int idx = 0; idx < 8; ++idx)
    {
      velocity += 47;

      int note = octave * 12 + scale[idx];
      int vel = velocity & 0x7F; //(note % 8) << 4;
      NoteOn(note, vel, 0);
      NoteOn(note, vel, 1);
      delay(250);
      NoteOff(0);
      NoteOff(1);
      delay(250);
    }
  }
}

void loop()
{
  // put your main code here, to run repeatedly:

  TestAll();
  delay(1000);

  return;

  ScanI2C(Serial);

  // TestDac();

  TestPlayScale();
}
