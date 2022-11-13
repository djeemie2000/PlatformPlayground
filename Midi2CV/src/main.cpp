#include <Arduino.h>
#include <Wire.h>

// #include "ScanI2C.h"
// #include "mcp4728.h"
#include "midinoteparser.h"
#include "testmidinoteparser.h"
#include "debugcounter.h"
#include "midi2gateclockapp.h"
#include "midi2cvapp.h"

#define DEBUGAPP 1
//#define DEBUGMIDI 1
#define CVAPP 1


#ifdef DEBUGAPP
DebugCounter debugCounter;
#endif


// MCP4728Dac Dac1;
MidiNoteParser midiNoteParser;

#ifndef CVAPP
Midi2GateClockApp app1;
Midi2GateClockApp app2;
#endif

#ifdef CVAPP
Midi2CVApp app3;
#endif

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(31250); // TODO midi Rx/Tx

  Serial.println("Midi16 Midi2Gate poc V0.6...");

  // test midi parser here?
  //TestAll();

#ifndef CVAPP
  app1.Begin(A6, 2, 4, 5, 6, 7, A0, A1, A2, A3);
  app2.Begin(A7, 3, 8, 9, 10, 11, 12, 13, A4, A5);
#endif

#ifdef CVAPP
  app3.Begin(A6, A7, 2, 3, 4, 5, 6, 7, A0, A1, A2, A3);
#endif

#ifdef DEBUGAPP
  debugCounter.Begin(2000);
#endif

  // setup midi2gate
  // setup midi2Clock


#ifdef DEBUGAPP

#ifndef CVAPP
  TestDigitalOutBank(app1.digitalOutBank, 1);
  TestDigitalOutBank(app2.digitalOutBank, 1);

  // TestLedOut(app1.ledOut, app1.statusLed1Pin, 1);
  // TestLedOut(app2.ledOut, app2.statusLed1Pin, 1);
  
  // TestAnalogButtonIn2(app1.buttons, 20);
  // TestAnalogButtonIn2(app2.buttons, 20);
#endif

#ifdef CVAPP
  TestDigitalOutBank(app3.digitalOutBank, 1);

  //TODO test MCP4822 DAC!
  TestMCP4822Bank(app3.dacBank, 2);
#endif

#endif

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
  // TODO regular interval for reading inputs?

  // limit max # bytes read
  const int maxNumBytes = 6;
  int numBytes = 0;
  while (Serial.available() && numBytes++ < maxNumBytes)
  {
    uint8_t byte = Serial.read();

#ifndef CVAPP
    app1.OnMidiMessage(byte);
    app2.OnMidiMessage(byte);
#endif

#ifdef DEBUGMIDI
    if(byte != 0xF8)
    {
      Serial.println(byte, HEX);
    }
#endif

    MidiVoiceMessage message;
    if (midiNoteParser.Parse(byte, message))
    {

#ifndef CVAPP
      app1.OnMidiMessage(message);
      app2.OnMidiMessage(message);
#endif

#ifdef CVAPP
      app3.OnMidiMessage(message);
#endif

#ifdef DEBUGAPP
      printVoiceMessage(message);
#endif
    }
  }

  // update based on millis
  unsigned long millies = millis();

#ifndef CVAPP
  app1.Update(millies);
  app2.Update(millies);
#endif

#ifdef CVAPP
  app3.Update(millies);
#endif

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

#ifndef CVAPP
    Serial.print(app1.mode);
    Serial.print(' ');
    Serial.println(app2.mode);
    
    app1.midi2Gate.PrintState();
    app2.midi2Gate.PrintState();
    app1.midi2Clock.PrintState();
    app2.midi2Clock.PrintState();
#endif

#ifdef CVAPP
    Serial.println(app3.mode);
    
    app3.midi2PG.PrintState();
    app3.midi2PGVG.PrintState();
#endif
  }
#endif

}
