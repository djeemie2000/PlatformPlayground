#include <Arduino.h>
#include "fastdigitaloutbank.h"
#include "fastdigitalinbank.h"
#include "gateoutbank.h"
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

// FastDigitalOutBank<8> gateOutBank;
// FastDigitalOutBank<3> ledOutBank;
// FastDigitalInBank<2> btnInBank;

// GateOutBank<8> gateOut;

MidiNoteParser midiNoteParser;
Midi2GateClockApp app1;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(31250);
  Serial.println("Midi2PVGC V2...");

  app1.Begin();

  app1.loadParams(128);

  // gateOutBank.Begin();

  // gateOutBank.Assign(0, A0);
  // gateOutBank.Assign(1, A1);
  // gateOutBank.Assign(2, A2);
  // gateOutBank.Assign(3, A3);
  // gateOutBank.Assign(4, 4);
  // gateOutBank.Assign(5, 5);
  // gateOutBank.Assign(6, 6);
  // gateOutBank.Assign(7, 7);

  // ledOutBank.Begin();

  // ledOutBank.Assign(0,8);
  // ledOutBank.Assign(1,9);
  // ledOutBank.Assign(2,10);

  // btnInBank.Begin();
  // btnInBank.Assign(0, 2, true);
  // btnInBank.Assign(1, 3, true);

  // gateOut.Begin();
}

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

void loop() 
{
    // put your main code here, to run repeatedly:
    TestHardware();
    return;

    const int maxNumBytes = 6;
    int numBytes = 0;
    while (Serial.available() && numBytes++ < maxNumBytes)
    {
      uint8_t byte = Serial.read();

      if(byte != 0xF8)//not midi clock
      {
        Serial.println(byte, HEX);
      }

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
    // delay(1);
  
}

