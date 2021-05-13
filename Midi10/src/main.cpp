#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

#include "midi10ui.h"
#include "single1handler.h"
#include "single2handler.h"
#include "mono2handler.h"
#include "mono4handler.h"
#include "poly2handler.h"
#include "poly4handler.h"
#include "MidiParser.h"
#include "clocksyncout.h"

Midi10UI Midi10UI;
MidiParser midiParserSerial;
Single1Handler single1Handler;
Single2Handler single2Handler;
Mono2Handler mono2Handler;
Mono4Handler mono4Handler;
Poly2Handler poly2Handler;
Poly4Handler poly4Handler;
ClockSyncHandler clockSyncHandler;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(31250); //115200);
  Serial.println("Midi8 v0.4...");

  Midi10UI.begin();
  clockSyncHandler.Begin(0, 1, 2, 3); //gate 0 1 led 2 3
}

void testUi()
{
  //TODO read dip switch for debug/test y/n
  testAnalogOutBank(Midi10UI.cvOut, 1);
  testDigitalOutBank(Midi10UI.gatesOut, 1);
  testLedOutBank(Midi10UI.ledsOut, 1);
}

void handleMidi(MidiHandler &midiHandler, MidiHandler &midiHandler2, MidiHandler &midiHandler3)
{
  // read serial midi in -> parser -> midi out
  // limit # bytes for performance issues
  int numBytesIn = Serial.available();
  const int maxNumBytesIn = 3;
  if (maxNumBytesIn < numBytesIn)
  {
    numBytesIn = maxNumBytesIn;
  }
  while (0 < numBytesIn)
  {
    uint8_t byte = Serial.read();
    //Serial.println(byte,HEX);
    midiParserSerial.Parse(byte, midiHandler);
    --numBytesIn;
  }
}

int paramSize()
{
  return 2 + single1Handler.paramSize() + single2Handler.paramSize() + mono2Handler.paramSize() + poly2Handler.paramSize() + mono4Handler.paramSize() + poly4Handler.paramSize();
}

void saveParams()
{
  int off = 0;
  // header
  EEPROM.update(off++, 'M');
  EEPROM.update(off++, '8');
  uint8_t version = 0x01;
  EEPROM.update(off++, version);
  uint8_t size = paramSize(); //Assumes <256!!
  EEPROM.update(off++, size);
  //  mode/grouping
  EEPROM.update(off++, Midi10UI.mode);
  EEPROM.update(off++, Midi10UI.grouping);
  // data
  single1Handler.saveParams(off);
  off += single1Handler.paramSize();
  single2Handler.saveParams(off);
  off += single2Handler.paramSize();
  mono2Handler.saveParams(off);
  off += mono2Handler.paramSize();
  poly2Handler.saveParams(off);
  off += poly2Handler.paramSize();
  mono4Handler.saveParams(off);
  off += mono4Handler.paramSize();
  poly4Handler.saveParams(off);
  off += poly4Handler.paramSize();
}

void loadParams()
{
  int off = 0;
  // header
  if (EEPROM.read(off++) == 'M' && EEPROM.read(off++) == '8')
  {
    uint8_t version = EEPROM.read(off++);
    uint8_t size = EEPROM.read(off++);
    if (version == 0x01 && 0x00 < size)
    {
      // mode/grouping
      char mode = EEPROM.read(off++);
      if (mode == Midi10UI::SingleMode || mode == Midi10UI::MonoMode || mode == Midi10UI::PolyMode)
      {
        Midi10UI.mode = mode;
      }
      char grouping = EEPROM.read(off++);
      if (grouping == Midi10UI::Grouping1 || grouping == Midi10UI::Grouping2 || grouping == Midi10UI::Grouping4)
      {
        Midi10UI.grouping = mode;
      }
      // data
      single1Handler.loadParams(off);
      off += single1Handler.paramSize();
      single2Handler.loadParams(off);
      off += single2Handler.paramSize();
      mono2Handler.loadParams(off);
      off += mono2Handler.paramSize();
      poly2Handler.loadParams(off);
      off += poly2Handler.paramSize();
      mono4Handler.loadParams(off);
      off += mono4Handler.paramSize();
      poly4Handler.loadParams(off);
      off += poly4Handler.paramSize();
    }
  }
}

void printParams()
{
  //TODO read dip switch for debug/test y/n
  Serial.println("params:");
  const int headerSize = 4;
  int size = headerSize + paramSize();
  for (int off = 0; off < size; ++off)
  {
    uint8_t val = EEPROM.read(off);
    Serial.print("0x");
    Serial.print(val, HEX);
    char c = val;
    Serial.print(" ");
    Serial.println(c);
  }
}

void loop()
{
  // put your main code here, to run repeatedly:

  //need to update enough times for the debouncing to work
  for (int repeat = 0; repeat < 100; ++repeat)
  {
    Midi10UI.update();
    delay(1);
  }
  //if(Midi10UI.modeBtn8.Get())
  {
    //testUi();
    printParams();
  }
  testLedOutBank(Midi10UI.ledsOut, 1);
  loadParams();

  unsigned long debugCntr = 0;
  while (true)
  {
    Midi10UI.update();
    if (Midi10UI.extraBtn.IsFalling())
    {
      saveParams();
      //TODO blink all leds before/after or all leds on during save
    }

    //TODO multi midi handler :
    //    clocksync
    //    single1
    //    single/mono/poly 2/4
    if (Midi10UI.grouping == Midi10UI::Grouping1)
    {
      // always single mode!
      handleMidi(single1Handler);
      single1Handler.updateUI(&Midi10UI);
    }
    else if (Midi10UI.grouping == Midi10UI::Grouping2)
    {
      if (Midi10UI.mode == Midi10UI::MonoMode)
      {
        handleMidi(mono2Handler);
        mono2Handler.updateUI(&Midi10UI);
      }
      else if (Midi10UI.mode == Midi10UI::PolyMode)
      {
        handleMidi(poly2Handler);
        poly2Handler.updateUI(&Midi10UI);
      }
      else if (Midi10UI.mode == Midi10UI::SingleMode)
      {
        handleMidi(single2Handler);
        single2Handler.updateUI(&Midi10UI);
      }
    }
    else if (Midi10UI.grouping == Midi10UI::Grouping4)
    {
      // only mono or poly
      if (Midi10UI.mode == Midi10UI::MonoMode)
      {
        handleMidi(mono4Handler);
        mono4Handler.updateUI(&Midi10UI);
      }
      else if (Midi10UI.mode == Midi10UI::PolyMode)
      {
        handleMidi(poly4Handler);
        poly4Handler.updateUI(&Midi10UI);
      }
    }

    ++debugCntr;
    if (2000 <= debugCntr)
    {
      {
        Serial.print(debugCntr);
        Serial.print(" -> ");
        Serial.print(millis());

        Serial.print(" ");
        Serial.print(Midi10UI.mode);
        Serial.println(Midi10UI.grouping);
      }
      debugCntr = 0;
    }
  }
}
