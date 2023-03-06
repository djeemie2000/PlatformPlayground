#pragma once
#include <Arduino.h>

#include "midi2gate.h"
#include "midi2clock.h"
#include "analogbuttonin2.h"
#include "debugcounter.h"
#include "digitaloutbank.h"
#include "gateoutbank.h"
#include "ledout.h"
#include "EEPROM.h"


struct Midi2GateClockApp
{
  int statusLed1Pin;
  AnalogButtonIn2 buttons;
  DigitalOutBank digitalOutBank;

  GateOutBank<8> gatesOut_midi2Gate;
  LedOut ledOut_midi2Gate;
  Midi2Gate<8> midi2Gate;
  
  GateOutBank<8> gatesOut_midi2Clock;
  LedOut ledOut_midi2clock;
  Midi2Clock midi2Clock;

  // mode : 0 = midi2gate 1=midi2clock
  uint8_t mode;

  // check for saveParams
  bool prevIsLearning;
  bool modeChanged;

  void Begin(uint8_t analogBtnInPin, uint8_t statusLedPin,
              uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, 
              uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7)
  {
    buttons.Begin(analogBtnInPin);

    statusLed1Pin = statusLedPin;
    pinMode(statusLed1Pin, OUTPUT);
    ledOut_midi2Gate.Begin();

    digitalOutBank.Begin(pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7);

    ledOut_midi2clock.Begin();
    gatesOut_midi2Gate.Begin();
    gatesOut_midi2Clock.Begin();

    midi2Gate.Begin(&gatesOut_midi2Gate, &ledOut_midi2Gate);
    midi2Clock.Begin(&gatesOut_midi2Clock, &ledOut_midi2clock);
    
    mode = 0;

    prevIsLearning = false;
    modeChanged = false;
  }

  void OnMidiMessage(uint8_t byte)
  {
    midi2Clock.OnMessage(byte);
  }

  void OnMidiMessage(MidiVoiceMessage& message)
  {
    midi2Gate.OnMessage(message);
  }

  void Update(unsigned long millies)
  {
    // read and decode analogin from 2 buttons via R2R dac
    buttons.update();
    if (buttons.IsClicked1())// not in mode 0??
    {
      midi2Gate.ToggleLearning();
    }
    if(buttons.IsClicked2())
    {
      mode = 1- mode;
      modeChanged = true;
    }

    uint8_t counter = millies >> 2;
    gatesOut_midi2Gate.Update(millies);
    gatesOut_midi2Clock.Update(millies);

    // apply depending on gate vs clock mode
    if(0 == mode)
    {
      gatesOut_midi2Gate.Apply(digitalOutBank);
      ledOut_midi2Gate.Apply(counter, statusLed1Pin);
    }
    else
    {
      gatesOut_midi2Clock.Apply(digitalOutBank);
      ledOut_midi2clock.Apply(counter, statusLed1Pin);
    }
  }

  void CheckSaveParams(int offset)
  {
    bool isLearning = midi2Gate.IsLearning();//clock does not learn
    if(modeChanged)
    {
      int off = offset;
      EEPROM.update(off++, 'M');
      EEPROM.update(off++, '2');
      EEPROM.update(off++, 'G');
      EEPROM.update(off++, 'C');

      EEPROM.update(off++, mode);
    }
    if(isLearning != prevIsLearning)
    {
      saveParams(offset);
    }
    modeChanged = false;
    prevIsLearning = isLearning;
  }

  void saveParams(int offset)
  {
      int off = offset;
      EEPROM.update(off++, 'M');
      EEPROM.update(off++, '2');
      EEPROM.update(off++, 'G');
      EEPROM.update(off++, 'C');

      EEPROM.update(off++, mode);

      midi2Gate.saveParams(off);
      off += midi2Gate.paramSize();

      midi2Clock.saveParams(off);
      off += midi2Clock.paramSize();
  }

  int paramSize() const
  {
      return 4+1+midi2Gate.paramSize()+midi2Clock.paramSize();
  }

  void loadParams(int offset)
  {
      int off = offset;
      if ('M' == EEPROM.read(off++) 
      && '2' == EEPROM.read(off++)
      && 'G' == EEPROM.read(off++)
      && 'C' == EEPROM.read(off++))
      {
          mode = EEPROM.read(off++);

          midi2Gate.loadParams(off);
          off += midi2Gate.paramSize();

          midi2Clock.loadParams(off);
          off += midi2Clock.paramSize();
      }
  }
};