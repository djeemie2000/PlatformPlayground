#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>

#include "midi8ui.h"
#include "single1handler.h"
#include "single2handler.h"
#include "mono2handler.h"
#include "mono4handler.h"
#include "poly2handler.h"
#include "poly4handler.h"
#include "MidiParser.h"
#include "clocksyncout.h"

Midi8UI midi8UI;
MidiParser midiParserSerial;
Single1Handler single1Handler;
Single2Handler single2Handler;
Mono2Handler mono2Handler;
Mono4Handler mono4Handler;
Poly2Handler poly2Handler;
Poly4Handler poly4Handler;
ClockSyncOut clockSyncOut;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(31250); //115200);
  Serial.println("Midi8 v0.4...");

  midi8UI.begin();
  clockSyncOut.Begin(2, 4); //gate 2 + gate 4
  mono4Handler.begin(&clockSyncOut);
  poly4Handler.begin(&clockSyncOut);
}

void testUi()
{
  //TODO read dip switch for debug/test y/n
  testAnalogOutBank(midi8UI.cvOut, 1);
  testDigitalOutBank(midi8UI.gatesOut, 1);
  testLedOutBank(midi8UI.ledsOut, 1);
}

void handleMidi(MidiHandler &midiHandler)
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
  EEPROM.update(off++, midi8UI.mode());
  EEPROM.update(off++, midi8UI.grouping());
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
      char grouping = EEPROM.read(off++);
      //TODO mapping between 2 char code and mode int
      midi8UI.setMode(mode, grouping);
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
    midi8UI.update();
    delay(1);
  }
  if (midi8UI.debug)
  {
    //testUi();
    printParams();
  }
  testLedOutBank(midi8UI.ledsOut, 1);
  loadParams();
  //show mode for a short time!
  midi8UI.showMode();

  unsigned long debugCntr = 0;
  while (true)
  {
    midi8UI.update();

    // handle mode toggle
    // very crude but it works
    if (midi8UI.extraBtn.IsFalling())
    {
      ++midi8UI.modeNbr;
      if (midi8UI.modeNbr > Midi8UI::Poly4Mode)
      {
        midi8UI.modeNbr = Midi8UI::Single1Mode;
      }
      midi8UI.showMode();
      saveParams();
    }

    if (midi8UI.modeNbr == Midi8UI::Single1Mode)
    {
      // always single mode!
      handleMidi(single1Handler);
      single1Handler.updateUI(&midi8UI);
    }
    else if (midi8UI.modeNbr == Midi8UI::Mono2Mode)
    {
      handleMidi(mono2Handler);
      mono2Handler.updateUI(&midi8UI);
    }
    else if (midi8UI.modeNbr == Midi8UI::Poly2Mode)
    {
      handleMidi(poly2Handler);
      poly2Handler.updateUI(&midi8UI);
    }
    else if (midi8UI.modeNbr == Midi8UI::Single2Mode)
    {
      handleMidi(single2Handler);
      single2Handler.updateUI(&midi8UI);
    }
    else if (midi8UI.modeNbr == Midi8UI::Mono4Mode)
    {
      handleMidi(mono4Handler);
      mono4Handler.updateUI(&midi8UI);
    }
    else if (midi8UI.modeNbr == Midi8UI::Poly4Mode)
    {
      handleMidi(poly4Handler);
      poly4Handler.updateUI(&midi8UI);
    }

    // midi learn -> off => trigger auto save
    if (midi8UI.learnMode.Get() == Midi8UI::NoLearn && midi8UI.learnMode.Previous() != Midi8UI::NoLearn)
    {
      if (midi8UI.debug)
      {
        Serial.println("AutoSave!");
      }
      saveParams();
    }

    ++debugCntr;
    if (2000 <= debugCntr)
    {
      if (midi8UI.debug)
      {
        Serial.print(debugCntr);
        Serial.print(" -> ");
        Serial.print(millis());

        Serial.print(" ");
        Serial.print(midi8UI.learnMode.Get());

        Serial.print(" ");
        Serial.print(midi8UI.modeNbr);

        Serial.print(" ");
        Serial.print(midi8UI.mode());
        Serial.println(midi8UI.grouping());
      }
      debugCntr = 0;
    }
  }
}
