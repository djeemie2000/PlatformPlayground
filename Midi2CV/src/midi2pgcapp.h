#pragma once
#include <Arduino.h>

#include "midi2pg.h"
#include "midi2gate.h"
#include "midi2clocktick.h"
#include "analogbuttonin2.h"
#include "digitaloutbank.h"
#include "gateoutbank.h"
#include "ledout.h"
#include "cvoutbank.h"
#include "mcp4822bank.h"
#include "EEPROM.h"

struct Midi2PGCApp
{
    int statusLed1Pin;
    int statusLed2Pin;
    AnalogButtonIn2 buttons1;
    AnalogButtonIn2 buttons2;
    DigitalOutBank digitalOutBank;
    MCP4822Bank dacBank;

    GateOutBank<4> gatesOut_Midi2PG;
    CVOutBank<4> cvOut_Midi2PG;
    LedOut ledOut_Midi2PG;
    Midi2PG<4> midi2PG;

    GateOutBank<6> gatesOut_midi2Gate;
    LedOut ledOut_midi2Gate;
    Midi2Gate<6> midi2Gate;

    GateOutBank<2> gatesOut_midi2Clock;
    Midi2ClockTick<2> midi2Clock; 

    // no mode (yet)
    
    // check for saveParams
    bool prevIsLearning;

    void Begin(uint8_t analogBtnInPin, uint8_t analogBtnInPin2,
               uint8_t statusLedPin, uint8_t statusLedPin2,
               uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3,
               uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7)
    {
        buttons1.Begin(analogBtnInPin);
        buttons2.Begin(analogBtnInPin2);

        statusLed1Pin = statusLedPin;
        pinMode(statusLed1Pin, OUTPUT);

        statusLed2Pin = statusLedPin2;
        pinMode(statusLed2Pin, OUTPUT);

        digitalOutBank.Begin(pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7);
        dacBank.Begin();

        gatesOut_Midi2PG.Begin();
        cvOut_Midi2PG.Begin();
        ledOut_Midi2PG.Begin();
        midi2PG.Begin(&gatesOut_Midi2PG, &ledOut_Midi2PG, &cvOut_Midi2PG);

        gatesOut_midi2Gate.Begin();
        ledOut_midi2Gate.Begin();
        midi2Gate.Begin(&gatesOut_midi2Gate, &ledOut_midi2Gate);

        gatesOut_midi2Clock.Begin();
        midi2Clock.Begin(&gatesOut_midi2Clock, 3, 6);// 3/6 => 24PPQ becomes 4PPQ (1/16th notes)  

        prevIsLearning = false;
    }

    void OnMidiMessage(uint8_t byte)
    {
        midi2Clock.OnMessage(byte);
    }

    void OnMidiMessage(MidiVoiceMessage& message)
    {
        midi2PG.OnMessage(message);
        midi2Gate.OnMessage(message);
    }

    void Update(unsigned long millies)
    {
        // read and decode analogin from 2 buttons via R2R dac
        buttons1.update();
        buttons2.update();

        if(buttons1.IsClicked1())
        {
            midi2PG.ToggleLearning();
        }

        if(buttons2.IsClicked1())
        {
            midi2Gate.ToggleLearning();
        }

        uint8_t counter = millies >> 2;
        gatesOut_Midi2PG.Update(millies);
        gatesOut_midi2Gate.Update(millies);

        gatesOut_Midi2PG.Apply(digitalOutBank);//TODO offset 0
        cvOut_Midi2PG.Apply(0, dacBank);
        ledOut_Midi2PG.Apply(counter, statusLed1Pin);

        gatesOut_midi2Gate.Apply(4, digitalOutBank, dacBank);
        ledOut_midi2Gate.Apply(counter, statusLed2Pin);

        gatesOut_midi2Clock.Apply(7, digitalOutBank, dacBank);
    }

    void CheckSaveParams(int offset)
    {
        bool isLearning = midi2PG.IsLearning() || midi2Gate.IsLearning();
        if(isLearning != prevIsLearning)
        {
            saveParams(offset);
        }
        prevIsLearning = isLearning;
    }

    void saveParams(int offset)
    {
        int off = offset;
        EEPROM.update(off++, 'M');
        EEPROM.update(off++, '2');
        EEPROM.update(off++, 'P');
        EEPROM.update(off++, 'G');

        uint8_t mode = 0;
        EEPROM.update(off++, mode);
        EEPROM.update(off++, mode);

        midi2PG.saveParams(off);
        off += midi2PG.paramSize();

        midi2Gate.saveParams(off);
        off += midi2Gate.paramSize();
    }

    int paramSize() const
    {
        return 4+2+midi2PG.paramSize()+midi2Gate.paramSize();
    }

    void loadParams(int offset)
    {
        int off = offset;
        if ('M' == EEPROM.read(off++) 
        && '2' == EEPROM.read(off++)
        && 'P' == EEPROM.read(off++)
        && 'G' == EEPROM.read(off++))
        {
            uint8_t mode1 = EEPROM.read(off++);
            uint8_t mode2 = EEPROM.read(off++);

            midi2PG.loadParams(off);
            off += midi2PG.paramSize();

            midi2Gate.loadParams(off);
            off += midi2Gate.paramSize();
        }
    }
};
