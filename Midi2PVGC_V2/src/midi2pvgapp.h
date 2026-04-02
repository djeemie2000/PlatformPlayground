#pragma once
#include <Arduino.h>

#include "midi2vg.h"
#include "midi2pg.h"
#include "midi2pgvg.h"
#include "fastdigitalinbank.h"
//#include "debugcounter.h"
#include "fastdigitaloutbank.h"
#include "mcp4728.h"
#include "gateoutbank.h"
#include "cvoutbank.h"
#include "ledout.h"
#include "EEPROM.h"


struct Midi2PVGApp
{
  FastDigitalInBank<2> buttonInBank;
  FastDigitalOutBank<4> gateOutBank;
  MCP4728Dac dac;
  FastDigitalOutBank<3> ledOutBank;

  GateOutBank<4> gatesOut_midi2VG;
  CVOutBank<4> cvOut_midi2VG;
  LedOut ledOut_midi2VG;
  Midi2VG<4> midi2VG;
  
  GateOutBank<4> gatesOut_midi2PG;
  CVOutBank<4> cvOut_midi2PG;
  LedOut ledOut_midi2PG;
  Midi2PG<4> midi2PG;

  GateOutBank<4> gatesOut_midi2PVG;
  CVOutBank<4> cvOut_midi2PVG;
  LedOut ledOut_midi2PVG;
  Midi2PGVG midi2PVG;//TODO size template 


  // mode : 0x04 = midi2gate 0X02=midi2clock
  const uint8_t midi2VGmode = 0x04;
  const uint8_t midi2PGmode = 0x02;
  const uint8_t midi2PVGmode = 0x01;
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

    ledOutBank.Begin();

    ledOutBank.Assign(0,8);
    ledOutBank.Assign(1,9);
    ledOutBank.Assign(2,10);

    buttonInBank.Begin();
    buttonInBank.Assign(0, 2, true);
    buttonInBank.Assign(1, 3, true);

    dac.Begin(MCP4728Dac::MCP4728_I2CADDR_DEFAULT);

    ledOut_midi2VG.Begin();
    gatesOut_midi2VG.Begin();

    ledOut_midi2PG.Begin();
    gatesOut_midi2PG.Begin();

    ledOut_midi2PVG.Begin();
    gatesOut_midi2PVG.Begin();

    midi2VG.Begin(&gatesOut_midi2VG, &cvOut_midi2VG, &ledOut_midi2VG);
    midi2PG.Begin(&gatesOut_midi2PG, &cvOut_midi2PG, &ledOut_midi2PG);
    midi2PVG.Begin(&gatesOut_midi2PVG, &cvOut_midi2PVG, &ledOut_midi2PVG);
    
    mode = midi2VGmode;

    prevIsLearning = false;
    modeChanged = false;
  }

  void OnMidiMessage(uint8_t byte)
  {
  }

  void OnMidiMessage(MidiVoiceMessage& message)
  {
    midi2VG.OnMessage(message);
    midi2PG.OnMessage(message);
    midi2PVG.OnMessage(message);
  }

  void Update(unsigned long millies)
  {
    buttonInBank.Update();
    
    if (buttonInBank.IsFalling(0))
    {
        if(mode == midi2VGmode)
        {
            #ifdef DEBUGAPP
            Serial.println("toggle learning VG");
            #endif
            midi2VG.ToggleLearning();
        }
        else if(mode == midi2PGmode)
        {
            #ifdef DEBUGAPP
            Serial.println("toggle learning PG");
            #endif
            midi2PG.ToggleLearning();
        }
        else if(mode == midi2PVGmode)
        {
            #ifdef DEBUGAPP
            Serial.println("toggle learning PVG");
            #endif
            midi2PVG.ToggleLearning();
        }
    } 

    if(buttonInBank.IsFalling(1))
    {
      // toggle mode
      if(mode == midi2VGmode)
      {
        mode = midi2PGmode;
      }
      else if(mode == midi2PGmode)
      {
        mode = midi2PVGmode;
      }
      else
      {
        mode = midi2VGmode;
      }
      modeChanged = true;

      #ifdef DEBUGAPP
      Serial.print("toggle mode ");
      Serial.println(mode);
      #endif
    }

    uint8_t counter = millies >> 2;
    gatesOut_midi2VG.Update(millies);
    gatesOut_midi2PG.Update(millies);
    gatesOut_midi2PVG.Update(millies);
    
    // apply depending on gate vs clock mode
    if(midi2VGmode == mode)
    {
      gatesOut_midi2VG.Apply(gateOutBank);
      cvOut_midi2VG.Apply(dac);
      ledOut_midi2VG.Apply(counter, 0, ledOutBank);
    }
    else if(midi2PGmode == mode)
    {
      gatesOut_midi2PG.Apply(gateOutBank);
      cvOut_midi2PG.Apply(dac);
      ledOut_midi2PG.Apply(counter, 1, ledOutBank);
    }
    else if(midi2PVGmode == mode)
    {
      gatesOut_midi2PVG.Apply(gateOutBank);
      cvOut_midi2PVG.Apply(dac);
      ledOut_midi2PVG.Apply(counter, 2, ledOutBank);
    }
  }

  void CheckSaveParams(int offset)
  {
    bool isLearning = midi2VG.IsLearning() || midi2PG.IsLearning() || midi2PVG.IsLearning(); 
    if(modeChanged)
    {
      int off = offset;
      EEPROM.update(off++, 'M');
      EEPROM.update(off++, '2');
      EEPROM.update(off++, 'P');
      EEPROM.update(off++, 'V');

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
      EEPROM.update(off++, 'P');
      EEPROM.update(off++, 'V');

      EEPROM.update(off++, mode);

      midi2VG.saveParams(off);
      off += midi2VG.paramSize();

      midi2PG.saveParams(off);
      off += midi2PG.paramSize();

      midi2PVG.saveParams(off);
      off += midi2PVG.paramSize();
  }

  int paramSize() const
  {
      return 4+1+midi2VG.paramSize()+midi2PG.paramSize()+midi2PVG.paramSize(); 
  }

  void loadParams(int offset)
  {
      #ifdef DEBUGAPP
      Serial.println("load params");
      #endif

      int off = offset;
      if ('M' == EEPROM.read(off++) 
      && '2' == EEPROM.read(off++)
      && 'P' == EEPROM.read(off++)
      && 'V' == EEPROM.read(off++))
      {
          mode = EEPROM.read(off++);

          midi2VG.loadParams(off);
          off += midi2VG.paramSize();

          midi2PG.loadParams(off);
          off += midi2PG.paramSize();

          midi2PVG.loadParams(off);
          off += midi2PVG.paramSize();
      }
  }
};
