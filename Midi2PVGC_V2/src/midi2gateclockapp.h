#pragma once
#include <Arduino.h>

#include "midi2gate.h"
#include "midi2clock.h"
#include "midi2gatefixed.h"
#include "fastdigitalinbank.h"
//#include "debugcounter.h"
#include "fastdigitaloutbank.h"
#include "gateoutbank.h"
#include "ledout.h"
#include "EEPROM.h"


struct Midi2GateClockApp
{
  int statusLed1Pin;
  FastDigitalInBank<2> buttonInBank;
  FastDigitalOutBank<8> gateOutBank;
  FastDigitalOutBank<3> ledOutBank;

  GateOutBank<8> gatesOut_midi2Gate;
  LedOut ledOut_midi2Gate;
  Midi2Gate<8> midi2Gate;
  
  GateOutBank<8> gatesOut_midi2Clock;
  LedOut ledOut_midi2clock;
  Midi2Clock midi2Clock;

  GateOutBank<8> gatesOut_midi2GateFixed;
  LedOut ledOut_midi2GateFixed;
  Midi2GateFixed<8> midi2GateFixed;


  // mode : 0x04 = midi2gate 0X02=midi2clock
  const uint8_t midi2gatemode = 0x04;
  const uint8_t midi2clockmode = 0x02;
  const uint8_t midi2gatefixedmode = 0x01;
  uint8_t mode;

  // check for saveParams
  bool prevIsLearning;
  bool modeChanged;

  void Begin()
  {
    gateOutBank.Begin();

    gateOutBank.Assign(0, A0);
    gateOutBank.Assign(1, A1);
    gateOutBank.Assign(2, A2);
    gateOutBank.Assign(3, A3);
    gateOutBank.Assign(4, 4);
    gateOutBank.Assign(5, 5);
    gateOutBank.Assign(6, 6);
    gateOutBank.Assign(7, 7);

    ledOutBank.Begin();

    ledOutBank.Assign(0,8);
    ledOutBank.Assign(1,9);
    ledOutBank.Assign(2,10);

    buttonInBank.Begin();
    buttonInBank.Assign(0, 2, true);
    buttonInBank.Assign(1, 3, true);


    ledOut_midi2Gate.Begin();
    gatesOut_midi2Gate.Begin();

    ledOut_midi2clock.Begin();
    gatesOut_midi2Clock.Begin();

    ledOut_midi2GateFixed.Begin();
    gatesOut_midi2GateFixed.Begin();

    midi2Gate.Begin(&gatesOut_midi2Gate, &ledOut_midi2Gate);
    midi2Clock.Begin(&gatesOut_midi2Clock, &ledOut_midi2clock);
    midi2GateFixed.Begin(&gatesOut_midi2GateFixed, &ledOut_midi2GateFixed);
    // midi channel 10, chromatic scale starting at midi note 36
    uint8_t midichannel = 0x09;
    midi2GateFixed.Assign(0, midichannel, 36);//C
    midi2GateFixed.Assign(1, midichannel, 38);//D
    midi2GateFixed.Assign(2, midichannel, 40);//E
    midi2GateFixed.Assign(3, midichannel, 41);//F
    midi2GateFixed.Assign(4, midichannel, 43);//G
    midi2GateFixed.Assign(5, midichannel, 45);//A
    midi2GateFixed.Assign(6, midichannel, 47);//B
    midi2GateFixed.Assign(7, midichannel, 48);//C
    
    mode = midi2gatemode;

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
    midi2GateFixed.OnMessage(message);
  }

  void Update(unsigned long millies)
  {
    buttonInBank.Update();
    
    if (buttonInBank.IsFalling(0) && mode == midi2gatemode)// only not in mode gate !!
    {
      #ifdef DEBUGAPP
      Serial.println("toggle learning");
      #endif
      midi2Gate.ToggleLearning();
    }

    if(buttonInBank.IsFalling(1))
    {
      // toggle mode
      if(mode == midi2gatemode)
      {
        mode = midi2clockmode;
      }
      else if(mode == midi2clockmode)
      {
        mode = midi2gatefixedmode;
      }
      else
      {
        mode = midi2gatemode;
      }
      modeChanged = true;

      #ifdef DEBUGAPP
      Serial.print("toggle mode ");
      Serial.println(mode);
      #endif
    }

    uint8_t counter = millies >> 2;
    gatesOut_midi2Gate.Update(millies);
    gatesOut_midi2Clock.Update(millies);
    gatesOut_midi2GateFixed.Update(millies);
    
    // apply depending on gate vs clock mode
    if(midi2gatemode == mode)
    {
      gatesOut_midi2Gate.Apply(gateOutBank);
      ledOut_midi2Gate.Apply(counter, 0, ledOutBank);
    }
    else if(midi2clockmode == mode)
    {
      gatesOut_midi2Clock.Apply(gateOutBank);
      ledOut_midi2clock.Apply(counter, 1, ledOutBank);
    }
    else if(midi2gatefixedmode == mode)
    {
      gatesOut_midi2GateFixed.Apply(gateOutBank);
      ledOut_midi2GateFixed.Apply(counter, 2, ledOutBank);
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
      #ifdef DEBUGAPP
      Serial.println("save params");
      #endif

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
      #ifdef DEBUGAPP
      Serial.println("load params");
      #endif

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
