#include <Arduino.h>
// #include "fastdigitaloutbank.h"
// #include "fastdigitalinbank.h"
// #include "gateoutbank.h"
#include "debugcounter.h"
#include "midivoicemessage.h"
#include "midinoteparser.h"
#include "midi2gateclockapp.h"
#include "midi2pvgapp.h"
#include "scani2c.h"

// hardware init and test
//
// serial debug out (31250 baudrate)
//
// 3x mode leds (blink, on, off, ...)
// 8x gate + led out
// 2x button in (debounce?)
//
// midi in + test
// 

//#define DEBUGAPP
//#define GCAPP

#ifdef DEBUGAPP
DebugCounter debugCounter;
#endif

MidiNoteParser midiNoteParser;
#ifdef GCAPP
Midi2GateClockApp app1;
#endif
#ifndef GCAPP
Midi2PVGApp app2;
#endif 

void TestHardware()
{
#ifdef GCAPP
  TestDigitalOutBank(app1.ledOutBank, 2);
  TestDigitalOutBank(app1.gateOutBank, 2);

  Serial.print("test buttons...");
  for(int repeat = 0; repeat<3000; ++repeat)
  {
    app1.buttonInBank.Update();
    PrintChanges(app1.buttonInBank);
    delay(1);
  }
  Serial.println(" done");
#else
  TestDigitalOutBank(app2.ledOutBank, 2);
  TestDigitalOutBank(app2.gateOutBank, 2);

  TestMCP4728(app2.dac, 1, true);
  TestMCP4728(app2.dac, 1, false);

  Serial.print("test buttons...");
  for(int repeat = 0; repeat<3000; ++repeat)
  {
    app2.buttonInBank.Update();
    PrintChanges(app2.buttonInBank);
    delay(1);
  }
  Serial.println(" done");

  AllOff(app2.dac);
#endif
}

void TestMidiIn()
{
  Serial.print("test midi in...");
  for(int repeat = 0; repeat<5000; ++repeat)
  {
    const int maxNumBytes = 6;
    int numBytes = 0;
    while (Serial.available() && numBytes++ < maxNumBytes)
    {
      uint8_t byte = Serial.read();

      if(byte != 0xF8)//not midi clock
      {
        Serial.println(byte, HEX);
      }

      MidiVoiceMessage message;
      if (midiNoteParser.Parse(byte, message))
      {
        printVoiceMessage(message);
      }
    }
    delay(1);
  }
  Serial.println(" done");

}

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(31250);
  Serial.println("Midi2PVGC V2 v0.8...");
  #ifdef GCAPP
  Serial.println("8 gates");
  #else
  Serial.println("4 gate 4 CV");
  #endif

#ifdef DEBUGAPP
  debugCounter.Begin(2000);
#endif

#ifdef GCAPP
  app1.Begin();
  app1.loadParams(128);
#else
  app2.Begin();
  app2.loadParams(192);
#endif

// TODO remove after debugging issue
// while(true)
// {
//    ScanI2C(Serial);
//    TestMCP4728(app2.dac, 1, true);
//    TestMCP4728(app2.dac, 1, false);
// }

#ifdef DEBUGAPP
  TestHardware();
#endif
}


void loop() 
{
    // put your main code here, to run repeatedly:
    //ScanI2C(Serial);
    //TestHardware();
    //TestMidiIn();
    //return;

    const int maxNumBytes = 6;
    int numBytes = 0;
    while (Serial.available() && numBytes++ < maxNumBytes)
    {
      uint8_t byte = Serial.read();

#ifdef DEBUGAPP
      if(byte != 0xF8)//not midi clock
      {
        Serial.println(byte, HEX);
      }
#endif

#ifdef GCAPP
      app1.OnMidiMessage(byte);
#endif

      MidiVoiceMessage message;
      if (midiNoteParser.Parse(byte, message))
      {
//        printVoiceMessage(message);
#ifdef GCAPP
        app1.OnMidiMessage(message);
#else
        app2.OnMidiMessage(message);
#endif
      }
    }

    // update based on millis
    unsigned long millies = millis();

#ifdef GCAPP
    app1.Update(millies);
    app1.CheckSaveParams(128);
#else
    app2.Update(millies);
    app2.CheckSaveParams(192);
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

#ifdef GCAPP
    Serial.print("Mode ");
    Serial.println(app1.mode);
    
    Serial.println("Gate");
    app1.midi2Gate.PrintState();
    Serial.println();
    app1.gatesOut_midi2Gate.PrintState();
    Serial.println();

    Serial.println("Clock"); 
    app1.midi2Clock.PrintState();
    Serial.println();
    app1.gatesOut_midi2Clock.PrintState();
    Serial.println();

    Serial.println("Fixed"); 
    app1.midi2GateFixed.PrintState();
    Serial.println();
    app1.gatesOut_midi2GateFixed.PrintState();
    Serial.println();
#endif
  }
  #endif
    
  delay(1);
}

