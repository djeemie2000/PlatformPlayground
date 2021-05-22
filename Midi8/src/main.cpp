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
#include "multimidihandler.h"

Midi8UI midi8UI;
MidiParser midiParserSerial;
Single1Handler single1Handler;
Single2Handler single2Handler;
Mono2Handler mono2Handler;
Mono4Handler mono4Handler;
Poly2Handler poly2Handler;
Poly4Handler poly4Handler;
ClockSyncOut clockSyncHandler;
MultiMidiHandler multiMidiHandler;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(31250); //115200);
  Serial.println("Midi8 v0.4...");

  midi8UI.begin();
  clockSyncHandler.Begin(8, 9); //gate 8 and 9
}

void testUi()
{
  //TODO read dip switch for debug/test y/n
  testAnalogOutBank(midi8UI.cvOut, 1);
  testDigitalOutBank(midi8UI.gatesOut, 1);
  testLedOutBank(midi8UI.ledsOut, 1);
}

void handleMidi(MidiHandler *midiHandler, MidiHandler *midiHandler2 = 0, MidiHandler *midiHandler3 = 0)
{
  multiMidiHandler.Begin(midiHandler, midiHandler2, midiHandler3);

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
    midiParserSerial.Parse(byte, multiMidiHandler);
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
  testDigitalOutBank(midi8UI.gatesOut, 1);
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
        midi8UI.modeNbr = Midi8UI::Single2Mode;
      }
      midi8UI.showMode();
      saveParams();
    }

    //handle midi learn toggle
    if (midi8UI.learnBtn.IsFalling())
    {
      if (midi8UI.learnMode == Midi8UI::NoLearn)
      {
        midi8UI.learnMode = Midi8UI::Learn1;
      }
      else if (midi8UI.learnMode == Midi8UI::Learn1)
      {
        midi8UI.learnMode = Midi8UI::Learn2;
      }
      else //if (midi8UI.learnMode == Midi8UI::Learn2)
      {
        midi8UI.learnMode = Midi8UI::NoLearn;
      }
    }

    // set learn on all handlers
    single1Handler.Learn(midi8UI.learnMode == Midi8UI::Learn2);

    single2Handler.Learn(midi8UI.learnMode == Midi8UI::Learn1);
    mono2Handler.Learn(midi8UI.learnMode == Midi8UI::Learn1);
    poly2Handler.Learn(midi8UI.learnMode == Midi8UI::Learn1);
    mono4Handler.Learn(midi8UI.learnMode == Midi8UI::Learn1);
    poly4Handler.Learn(midi8UI.learnMode == Midi8UI::Learn1);

    //TODO bool isLearning -> fill in on current handler after handleMidi
    bool learn1 = false;
    if (midi8UI.modeNbr == Midi8UI::Mono2Mode)
    {
      handleMidi(&mono2Handler, &single1Handler, &clockSyncHandler);
      learn1 = mono2Handler.IsLearning();
      mono2Handler.updateUI(&midi8UI);
    }
    else if (midi8UI.modeNbr == Midi8UI::Poly2Mode)
    {
      handleMidi(&poly2Handler, &single1Handler, &clockSyncHandler);
      learn1 = poly2Handler.IsLearning();
      poly2Handler.updateUI(&midi8UI);
    }
    else if (midi8UI.modeNbr == Midi8UI::Single2Mode)
    {
      handleMidi(&single2Handler, &single1Handler, &clockSyncHandler);
      learn1 = single2Handler.IsLearning();
      single2Handler.updateUI(&midi8UI);
    }
    else if (midi8UI.modeNbr == Midi8UI::Mono4Mode)
    {
      handleMidi(&mono4Handler, &single1Handler, &clockSyncHandler);
      learn1 = mono4Handler.IsLearning();
      mono4Handler.updateUI(&midi8UI);
    }
    else if (midi8UI.modeNbr == Midi8UI::Poly4Mode)
    {
      handleMidi(&poly4Handler, &single1Handler, &clockSyncHandler);
      learn1 = poly4Handler.IsLearning();
      poly4Handler.updateUI(&midi8UI);
    }
    // both handlers below were called upon handleMidi()
    single1Handler.updateUI(&midi8UI);
    clockSyncHandler.updateUI(&midi8UI);

    if (midi8UI.learnMode == Midi8UI::Learn1)
    {
      // if Learn1 mode and current handler is no longer learning after handleMidi => trigger auto save, learn mode to no learn
      if (!learn1)
      {
        midi8UI.learnMode = Midi8UI::NoLearn;
        if (midi8UI.debug)
        {
          Serial.println("AutoSave!");
        }
        saveParams();
      }
    }
    else if (midi8UI.learnMode == Midi8UI::Learn2)
    {
      // if Learn2 mode and sngle1  handler is no longer learning after handleMidi => trigger auto save, learn mode to no learn
      if (!single1Handler.IsLearning())
      {
        midi8UI.learnMode = Midi8UI::NoLearn;
        if (midi8UI.debug)
        {
          Serial.println("AutoSave!");
        }
        saveParams();
      }
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
        Serial.print(midi8UI.learnMode);

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
