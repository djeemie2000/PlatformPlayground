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

Midi10UI midi10UI;
MidiParser midiParserSerial;
Single1Handler single1Handler;
Single2Handler single2Handler;
Mono2Handler mono2Handler;
Mono4Handler mono4Handler;
Poly2Handler poly2Handler;
Poly4Handler poly4Handler;
ClockSyncHandler clockSyncHandler;
MultiMidiHandler multiMidiHandler;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(31250); //115200);
  Serial.println("Midi10 v0.4...");

  midi10UI.begin();
  clockSyncHandler.Begin(2, 3, 0, 1, 4, 5); //gate 2 3 led 0 1 gate 4 5
}

void testUi()
{
  //TODO read dip switch for debug/test y/n
  testAnalogOutBank(midi10UI.cvOut, 1);
  testDigitalOutBank(midi10UI.clockDigitalOut, 1);
  testLedOutBank(midi10UI.gateDigitalOut, 1);
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
  EEPROM.update(off++, midi10UI.mode());
  EEPROM.update(off++, midi10UI.grouping());
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
      midi10UI.setMode(mode, grouping);
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
  // while (true)
  // {
  //   Serial.println("Test Clock");
  //   testDigitalOutBank(midi10UI.clockDigitalOut, 1);
  //   Serial.println("Test digital");
  //   testLedOutBank(midi10UI.gateDigitalOut, 1);
  // }
  //need to update enough times for the debouncing to work
  for (int repeat = 0; repeat < 100; ++repeat)
  {
    midi10UI.update();
    delay(1);
  }
  testLedOutBank(midi10UI.gateDigitalOut, 1);
  testDigitalOutBank(midi10UI.clockDigitalOut, 1);
  loadParams();
  if (midi10UI.debug)
  {
    //testUi();
    printParams();
  }

  //show mode for a short time!
  midi10UI.showMode();

  unsigned long debugCntr = 0;
  while (true)
  {
    midi10UI.update();

    if (midi10UI.extraBtn.IsFalling())
    {
      ++midi10UI.modeNbr;
      if (midi10UI.modeNbr > Midi10UI::Poly4Mode)
      {
        midi10UI.modeNbr = Midi10UI::Single2Mode;
      }
      midi10UI.showMode();
      saveParams();
    }

    //handle midi learn toggle
    if (midi10UI.learnBtn.IsFalling())
    {
      if (midi10UI.learnMode == Midi10UI::NoLearn)
      {
        midi10UI.learnMode = Midi10UI::Learn;
      }
      else
      {
        midi10UI.learnMode = Midi10UI::NoLearn;
      }
    }

    if (midi10UI.learnBtn2.IsFalling())
    {
      if (midi10UI.learnMode2 == Midi10UI::NoLearn)
      {
        midi10UI.learnMode2 = Midi10UI::Learn;
      }
      else
      {
        midi10UI.learnMode2 = Midi10UI::NoLearn;
      }
    }

    // set learn on all handlers
    single2Handler.Learn(midi10UI.learnMode == Midi10UI::Learn);
    mono2Handler.Learn(midi10UI.learnMode == Midi10UI::Learn);
    poly2Handler.Learn(midi10UI.learnMode == Midi10UI::Learn);
    mono4Handler.Learn(midi10UI.learnMode == Midi10UI::Learn);
    poly4Handler.Learn(midi10UI.learnMode == Midi10UI::Learn);

    single1Handler.Learn(midi10UI.learnMode2 == Midi10UI::Learn);

    // bool isLearning -> fill in on current handler after handleMidi
    bool learn1 = false;
    if (midi10UI.modeNbr == Midi10UI::Mono2Mode)
    {
      handleMidi(&mono2Handler, &single1Handler, &clockSyncHandler);
      learn1 = mono2Handler.IsLearning();
      mono2Handler.updateUI(&midi10UI);
    }
    else if (midi10UI.modeNbr == Midi10UI::Poly2Mode)
    {
      handleMidi(&poly2Handler, &single1Handler, &clockSyncHandler);
      learn1 = poly2Handler.IsLearning();
      poly2Handler.updateUI(&midi10UI);
    }
    else if (midi10UI.modeNbr == Midi10UI::Single2Mode)
    {
      handleMidi(&single2Handler, &single1Handler, &clockSyncHandler);
      learn1 = single2Handler.IsLearning();
      single2Handler.updateUI(&midi10UI);
    }
    else if (midi10UI.modeNbr == Midi10UI::Mono4Mode)
    {
      handleMidi(&mono4Handler, &single1Handler, &clockSyncHandler);
      learn1 = mono4Handler.IsLearning();
      mono4Handler.updateUI(&midi10UI);
    }
    else if (midi10UI.modeNbr == Midi10UI::Poly4Mode)
    {
      handleMidi(&poly4Handler, &single1Handler, &clockSyncHandler);
      learn1 = poly4Handler.IsLearning();
      poly4Handler.updateUI(&midi10UI);
    }
    // both handlers below were called upon handleMidi()
    single1Handler.updateUI(&midi10UI);
    clockSyncHandler.updateUI(&midi10UI);

    if (midi10UI.learnMode == Midi10UI::Learn)
    {
      // if Learn mode and current handler is no longer learning after handleMidi => trigger auto save, learn mode to no learn
      if (!learn1)
      {
        midi10UI.learnMode = Midi10UI::NoLearn;
        if (midi10UI.debug)
        {
          Serial.println("AutoSave!");
        }
        saveParams();
      }
    }

    if (midi10UI.learnMode2 == Midi10UI::Learn)
    {
      // if Learn mode and single1  handler is no longer learning after handleMidi => trigger auto save, learn mode to no learn
      if (!single1Handler.IsLearning())
      {
        midi10UI.learnMode2 = Midi10UI::NoLearn;
        if (midi10UI.debug)
        {
          Serial.println("AutoSave!");
        }
        saveParams();
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
        Serial.print(midi10UI.learnMode);
        Serial.print(midi10UI.learnMode2);

        Serial.print(" ");
        Serial.print(midi10UI.modeNbr);

        Serial.print(" ");
        Serial.print(midi10UI.mode());
        Serial.println(midi10UI.grouping());
      }
      debugCntr = 0;
    }
  }
}
