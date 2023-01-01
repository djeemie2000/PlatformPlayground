#pragma once
#include <Arduino.h>

#include "midi2pg.h"
#include "midi2pgvg.h"
#include "analogbuttonin2.h"
#include "digitaloutbank.h"
#include "gateoutbank.h"
#include "ledout.h"
#include "cvoutbank.h"
#include "mcp4822bank.h"
#include "EEPROM.h"

struct Midi2CVApp
{
    int statusLed1Pin;
    int statusLed2Pin;
    AnalogButtonIn2 buttons1;
    AnalogButtonIn2 buttons2;
    DigitalOutBank digitalOutBank;
    MCP4822Bank dacBank;

    GateOutBank gatesOut_Midi2PG;
    CVOutBank cvOut_Midi2PG;
    LedOut ledOut_Midi2PG;
    Midi2PG midi2PG;

    GateOutBank gatesOut_Midi2PGVG;
    CVOutBank cvOut_Midi2PGVG;
    LedOut ledOut_Midi2PGVG;
    Midi2PGVG midi2PGVG;

    // mode : 0 = PG mode 1 = PGVG mode
    uint8_t mode;
    LedOut ledOut_Mode;
    
    // check for saveParams
    bool prevIsLearning;
    bool modeChanged;

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
        ledOut_Mode.Begin();

        digitalOutBank.Begin(pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7);
        dacBank.Begin();

        ledOut_Midi2PG.Begin();
        gatesOut_Midi2PG.Begin();
        cvOut_Midi2PG.Begin();
        midi2PG.Begin(&gatesOut_Midi2PG, &ledOut_Midi2PG, &cvOut_Midi2PG);

        ledOut_Midi2PGVG.Begin();
        gatesOut_Midi2PGVG.Begin();
        cvOut_Midi2PGVG.Begin();
        midi2PGVG.Begin(&gatesOut_Midi2PGVG, &ledOut_Midi2PGVG, &cvOut_Midi2PGVG);

        mode = 0;
        // mode = 0 => led2 off
        // mode = 1 => led2 on
        // led1 is hendled by Midi2PG / Midi2PGVG (learning)
        ledOut_Mode.LedOff();

        prevIsLearning = false;
        modeChanged = false;
    }

    void OnMidiMessage(uint8_t byte)
    {
    }

    void OnMidiMessage(MidiVoiceMessage &message)
    {
        midi2PG.OnMessage(message);
        midi2PGVG.OnMessage(message);
    }

    void Update(unsigned long millies)
    {
        // read and decode analogin from 2 buttons via R2R dac
        buttons1.update();
        buttons2.update();
        
        if (buttons2.IsClicked1())
        {
            // toggle mode button

            // stop learning in original mode
            if (0 == mode)
            {
                if (midi2PG.IsLearning())
                {
                    midi2PG.ToggleLearning();
                }
            }
            else
            {
                if (midi2PGVG.IsLearning())
                {
                    midi2PGVG.ToggleLearning();
                }
            }

            // toggle mode
            mode = 1 - mode;
            modeChanged = true;
        }

        if (buttons1.IsClicked1())
        {
            // toggle learning on/off

            // depending on mode, handle learning by PG or PGVG?
            if (0 == mode)
            {
                midi2PG.ToggleLearning();
            }
            else
            {
                midi2PGVG.ToggleLearning();
            }
        }

        uint8_t counter = millies >> 2;
        gatesOut_Midi2PG.Update(millies);
        gatesOut_Midi2PGVG.Update(millies);

        // apply depending on gate vs clock mode
        if (0 == mode)
        {
            gatesOut_Midi2PG.Apply(digitalOutBank);
            ledOut_Midi2PG.Apply(counter, statusLed1Pin);
            cvOut_Midi2PG.Apply(dacBank);

            ledOut_Mode.LedOff();
            ledOut_Mode.Apply(counter, statusLed2Pin);
        }
        else
        {
            gatesOut_Midi2PGVG.Apply(digitalOutBank);
            ledOut_Midi2PGVG.Apply(counter, statusLed1Pin);
            cvOut_Midi2PGVG.Apply(dacBank);

            ledOut_Mode.LedOn();
            ledOut_Mode.Apply(counter, statusLed2Pin);
        }
    }

    void CheckSaveParams(int offset)
    {
        bool isLearning = midi2PG.IsLearning() || midi2PGVG.IsLearning();
        if(modeChanged)
        {
            int off = offset;
            EEPROM.update(off++, 'M');
            EEPROM.update(off++, '2');
            EEPROM.update(off++, 'C');
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
        int off = offset;
        EEPROM.update(off++, 'M');
        EEPROM.update(off++, '2');
        EEPROM.update(off++, 'C');
        EEPROM.update(off++, 'V');

        EEPROM.update(off++, mode);

        midi2PG.saveParams(off);
        off += midi2PG.paramSize();

        midi2PGVG.saveParams(off);
        off += midi2PGVG.paramSize();
    }

    int paramSize() const
    {
        return 4+1+midi2PG.paramSize()+midi2PGVG.paramSize();
    }

    void loadParams(int offset)
    {
        int off = offset;
        if ('M' == EEPROM.read(off++) 
        && '2' == EEPROM.read(off++)
        && 'C' == EEPROM.read(off++)
        && 'V' == EEPROM.read(off++))
        {
            mode = EEPROM.read(off++);

            midi2PG.loadParams(off);
            off += midi2PG.paramSize();

            midi2PGVG.loadParams(off);
            off += midi2PGVG.paramSize();
        }
    }

};
