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
#include "digitaloutbank.h"
#include "gateoutbank.h"
#include "ledout.h"

//#define DEBUGAPP 1
//#define DEBUGMIDI 1

const int statusLed1Pin = 2;
const int statusLed2Pin = 3;

AnalogButtonIn2 buttons1;
AnalogButtonIn2 buttons2;
DigitalOutBank digitalOutBank1;
DigitalOutBank digitalOutBank2;
GateOutBank midi2Gate1Gates;
GateOutBank midi2Gate2Gates;
GateOutBank midi2Clock1Gates;
GateOutBank midi2Clock2Gates;
LedOut ledOut1;
LedOut ledOut2;
LedOut clockLedOut1;
LedOut clockLedOut2;

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
  digitalOutBank1.Begin(4, 5, 6, 7, A0, A1, A2, A3);
  digitalOutBank2.Begin(8, 9, 10, 11, 12, 13, A4, A5);
  ledOut1.Begin();
  ledOut2.Begin();
  clockLedOut1.Begin();
  clockLedOut2.Begin();
  midi2Gate1Gates.Begin();
  midi2Gate2Gates.Begin();
  midi2Clock1Gates.Begin();
  midi2Clock2Gates.Begin();

#ifdef DEBUGAPP
  debugCounter.Begin(10000);
#endif

  // setup midi2gate
  midi2Gate1.Begin(&midi2Gate1Gates, &ledOut1);
  midi2Gate2.Begin(&midi2Gate2Gates, &ledOut2);
  // setup midi2Clock
  midi2Clock1.Begin(&midi2Clock1Gates, &clockLedOut1);
  midi2Clock1.Begin(&midi2Clock2Gates, &clockLedOut2);

  TestAnalogButtonIn2(buttons1, 100);
  TestAnalogButtonIn2(buttons2, 100);

  // // Dac1.Begin(MCP4728Dac::MCP4728_I2CADDR_DEFAULT);
  // // pitch channels : internal ref + gain x2 => 0-4.096 V
  // // velocity channels : VDD (5V) ref + gain x1 => 0-5V
  // Dac1.ConfigureChannel(0, MCP4728Dac::vref::MCP4728_VREF_INTERNAL, MCP4728Dac::gain::MCP4728_GAIN_2X);
  // Dac1.ConfigureChannel(1, MCP4728Dac::vref::MCP4728_VREF_VDD, MCP4728Dac::gain::MCP4728_GAIN_1X);
  // Dac1.ConfigureChannel(2, MCP4728Dac::vref::MCP4728_VREF_INTERNAL, MCP4728Dac::gain::MCP4728_GAIN_2X);
  // Dac1.ConfigureChannel(3, MCP4728Dac::vref::MCP4728_VREF_VDD, MCP4728Dac::gain::MCP4728_GAIN_1X);
}

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

  // update based on millis
  unsigned long millies = millis();
  midi2Gate1Gates.Update(millies);
  midi2Gate2Gates.Update(millies);

  midi2Clock1Gates.Update(millies);
  midi2Clock2Gates.Update(millies);

  // apply depending on gate vs clock mode
  // gate mode
  ledOut1.Apply(statusLed1Pin, millies >> 2);
  ledOut2.Apply(statusLed2Pin, millies >> 2);
  midi2Gate1Gates.Apply(digitalOutBank1);
  midi2Gate2Gates.Apply(digitalOutBank1);
 // ifclock mode =>
  // clockLedOut1.Apply(statusLed1Pin, millies >> 2);
  // clockLedOut2.Apply(statusLed2Pin, millies >> 2);
  // midi2Clock1Gates.Apply(digitalOutBank1);
  // midi2Clock2Gates.Apply(digitalOutBank1);
 

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
