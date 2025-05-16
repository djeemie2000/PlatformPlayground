#include <Arduino.h>
// #include "fastdigitaloutbank.h"
// #include "fastdigitalinbank.h"
// #include "gateoutbank.h"
#include "debugcounter.h"
#include "midivoicemessage.h"
#include "midinoteparser.h"
#include "midi2gateclockapp.h"

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

#ifdef DEBUGAPP
DebugCounter debugCounter;
#endif

MidiNoteParser midiNoteParser;
Midi2GateClockApp app1;

void TestHardware()
{
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
  Serial.println("Midi2PVGC V2 v0.5...");

#ifdef DEBUGAPP
  debugCounter.Begin(2000);
#endif

  app1.Begin();

  app1.loadParams(128);

#ifdef DEBUGAPP
  TestHardware();
#endif
}


void loop() 
{
    // put your main code here, to run repeatedly:
//    TestMidiIn();
//    return;

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

      app1.OnMidiMessage(byte);

      MidiVoiceMessage message;
      if (midiNoteParser.Parse(byte, message))
      {
//        printVoiceMessage(message);
        app1.OnMidiMessage(message);
      }
    }

    // update based on millis
    unsigned long millies = millis();

    app1.Update(millies);

    app1.CheckSaveParams(128);

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
  }
  #endif
    
  delay(1);
}

