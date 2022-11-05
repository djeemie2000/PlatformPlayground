#include <Arduino.h>
#include <Wire.h>

// #include "ScanI2C.h"
// #include "mcp4728.h"
#include "midinoteparser.h"
#include "testmidinoteparser.h"
#include "midi2gate.h"
#include "midi2clock.h"
#include "analogbuttonin2.h"
#include "debugcounter.h"
#include "gateoutbank.h"

//#define DEBUGAPP 1
//#define DEBUGMIDI 1

const int statusLed1Pin = 2;
const int statusLed2Pin = 3;

AnalogButtonIn2 buttons1;
AnalogButtonIn2 buttons2;
GateOutBank midi2Gate1Out;
GateOutBank midi2Gate2Out;

#ifdef DEBUGAPP
DebugCounter debugCounter;
#endif

// MCP4728Dac Dac1;
MidiNoteParser midiNoteParser;
Midi2Gate midi2Gate1;
Midi2Gate midi2Gate2;
Midi2Clock midi2Clock1;
Midi2Clock midi2Clock2;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(31250); // TODO midi Rx/Tx

  Serial.println("Midi16 Midi2Gate poc ...");

  // test here?
  TestAll();

  // setup common
  buttons1.Begin(A6);
  buttons2.Begin(A7);
  midi2Gate1Out.Begin(4, 5, 6, 7, A0, A1, A2, A3, statusLed1Pin);
  midi2Gate2Out.Begin(8, 9, 10, 11, 12, 13, A4, A5, statusLed2Pin);

#ifdef DEBUGAPP
  debugCounter.Begin(10000);
#endif

  // setup midi2gate
  midi2Gate1.Begin(&midi2Gate1Out);
  midi2Gate2.Begin(&midi2Gate2Out);

  TestAnalogButtonIn2(buttons1, 100);
  TestAnalogButtonIn2(buttons2, 100);

  // PWM out -> RC filter 104 47k -> buffer opamp
  // setupNormalPwm();

  // pinMode(gatePin0, OUTPUT);
  // pinMode(gatePin1, OUTPUT);
  // // Dac1.Begin(MCP4728Dac::MCP4728_I2CADDR_DEFAULT);
  // // pitch channels : internal ref + gain x2 => 0-4.096 V
  // // velocity channels : VDD (5V) ref + gain x1 => 0-5V
  // Dac1.ConfigureChannel(0, MCP4728Dac::vref::MCP4728_VREF_INTERNAL, MCP4728Dac::gain::MCP4728_GAIN_2X);
  // Dac1.ConfigureChannel(1, MCP4728Dac::vref::MCP4728_VREF_VDD, MCP4728Dac::gain::MCP4728_GAIN_1X);
  // Dac1.ConfigureChannel(2, MCP4728Dac::vref::MCP4728_VREF_INTERNAL, MCP4728Dac::gain::MCP4728_GAIN_2X);
  // Dac1.ConfigureChannel(3, MCP4728Dac::vref::MCP4728_VREF_VDD, MCP4728Dac::gain::MCP4728_GAIN_1X);
}

// void NoteOn(int midiNote, int velocity, int channel)
// {
//   Serial.print("ch ");
//   Serial.print(channel);
//   Serial.print(" : note on ");
//   Serial.print(midiNote);
//   Serial.print(" vel ");
//   Serial.println(velocity);

//   if (channel == 0 || channel == 1)
//   {
//     // max note = 4096 / 1000 * 12 = 49
//     int midiNoteInternal = midiNote <= 49 ? midiNote : 49;
//     int32_t value1 = midiNoteInternal * 1000l / 12;
//     Dac1.SetValue(channel * 2, value1);

//     // velocity [0,127] 7 bits -> scale up to 12 bits
//     int value2 = velocity << 5;
//     Dac1.SetValue(channel * 2 + 1, value2);

//     Dac1.Update();

//     // gate on
//     if (channel == 0)
//     {
//       digitalWrite(gatePin0, HIGH);
//     }
//     else if (channel == 1)
//     {
//       digitalWrite(gatePin1, HIGH);
//     }
//   }
// }

// void NoteOff(int channel)
// {
//   Serial.print("ch ");
//   Serial.print(channel);
//   Serial.println(" : note off ");

//   // keep pitch cv out
//   // keep velocity cv out
//   // gate off
//   if (channel == 0)
//   {
//     digitalWrite(gatePin0, LOW);
//   }
//   else if (channel == 1)
//   {
//     digitalWrite(gatePin1, LOW);
//   }
// }

// void TestPlayScale()
// {
//   int scale[] = {0, 2, 4, 5, 7, 9, 11, 12};
//   int velocity = 0;
//   for (int octave = 0; octave < 4; ++octave)
//   {
//     for (int idx = 0; idx < 8; ++idx)
//     {
//       velocity += 47;

//       int note = octave * 12 + scale[idx];
//       int vel = velocity & 0x7F; //(note % 8) << 4;
//       NoteOn(note, vel, 0);
//       NoteOn(note, vel, 1);
//       delay(250);
//       NoteOff(0);
//       NoteOff(1);
//       delay(250);
//     }
//   }
// }

void printVoiceMessage(MidiVoiceMessage &message)
{
  Serial.print(message.StatusByte, HEX);
  Serial.print(' ');
  Serial.print(message.Param1, HEX);
  Serial.print(' ');
  Serial.println(message.Param2, HEX);
}

void loop()
{
  // put your main code here, to run repeatedly:

  //  ScanI2C(Serial);
  // TestDac();
  //  TestPlayScale();

  // TODO regular interval for reading inputs

  // read buttons in for toggle midi learn
  // read and decode analogin from 2 buttons via R2R dac
  buttons1.update();
  buttons2.update();
  // TODO debounce??

  if (buttons1.IsClicked1())
  {
    midi2Gate1.ToggleLearning();
  }

  if (buttons2.IsClicked1())
  {
    midi2Gate2.ToggleLearning();
  }



  // limit max # bytes read
  const int maxNumBytes = 6;
  int numBytes = 0;
  while (Serial.available() && numBytes++ < maxNumBytes)
  {
    uint8_t byte = Serial.read();

    midi2Clock1.OnMessage(byte);
    midi2Clock2.OnMessage(byte);

#ifdef DEBUGMIDI
    if(byte != 0xF8)
    {
      Serial.println(byte, HEX);
    }
#endif

    MidiVoiceMessage message;
    if (midiNoteParser.Parse(byte, message))
    {
      midi2Gate1.OnMessage(message);
      midi2Gate2.OnMessage(message);
      
#ifdef DEBUGAPP
      printVoiceMessage(message);
#endif
    }
  }

  // TODO ledoutbank with on/off/blink
  // onTick based on millis
  unsigned long millies = millis();
  midi2Gate1.OnTick(millies >> 2);
  midi2Gate2.OnTick(millies >> 2);

  midi2Gate1Out.Update(millies >> 2);
  midi2Gate2Out.Update(millies >> 2);

#ifdef DEBUGAPP
  unsigned long elapsedMillis = 0;
  if (debugCounter.Tick(millies, elapsedMillis))
  {
    // print state + elapsed time once per 1000 loops or so
    Serial.print('c');
    Serial.println(debugCounter.GetPeriod(), DEC);
    Serial.print('e');
    Serial.println(elapsedMillis, DEC);
    Serial.println();

    midi2Gate1.PrintState();
    midi2Gate2.PrintState();
  }
#endif

}
