#pragma once

#include <Arduino.h>
#include "analogoutbank.h"
#include "ledoutbank.h"
#include "DigitalOutBank.h"
#include "buttonin.h"

struct Midi10UI
{
    //TODO mapping between 2 char code and int modeNbr
    static const char SingleMode = 'S';
    static const char MonoMode = 'M';
    static const char PolyMode = 'P';

    static const char Grouping1 = '1';
    static const char Grouping2 = '2';
    static const char Grouping4 = '4';

    static const int Single1Mode = 0;
    static const int Single2Mode = 1;
    static const int Mono2Mode = 2;
    static const int Poly2Mode = 3;
    static const int Mono4Mode = 4;
    static const int Poly4Mode = 5;

    // grouping (1/2/4) + mode (single/mono/poly)
    int modeNbr;

    bool debug;

    static const int NoLearn = 0;
    static const int Learn = 1;
    int learnMode;
    int learnMode2;

    DigitalOutBank clockDigitalOut; //(6 = 2 led + 4 gate);
    AnalogOutBank cvOut;            //(4);// 2x DAC => use CS pins 10,9
    LedOutBank gateDigitalOut;      //(16 = 8 gate + 8 led);//use CS pin 8

    // toggle learn on/off
    ButtonIn learnBtn;  // pin A0
    ButtonIn learnBtn2; // pin A1
    ButtonIn extraBtn;  // pin A2
    ButtonIn extraBtn2; // pin A3

    //TODO analogRead ==0 or not // pin A6
    //TODO analogRead ==0 or not // pin A7

    Midi10UI()
        : modeNbr(Single1Mode), debug(true), learnMode(0), learnMode2(0), clockDigitalOut(), cvOut(), gateDigitalOut(8), learnBtn(), learnBtn2(), extraBtn(), extraBtn2()
    {
    }

    void begin()
    {
        modeNbr = Single1Mode;
        learnMode = NoLearn;

        // led out 2 3
        // gate out 4 5 6 7
        clockDigitalOut.begin(2, 3, 4, 5, 6, 7);
        cvOut.begin(4);
        gateDigitalOut.begin();
        const int debounce = 30;
        learnBtn.begin(A0, debounce);
        learnBtn2.begin(A1, debounce);
        extraBtn.begin(A2, debounce);
        extraBtn2.begin(A3, debounce);
    }

    void update()
    {
        int blink = (millis() >> 7) & 1; //period 256 msec
        clockDigitalOut.update(blink);
        cvOut.update();
        gateDigitalOut.update(blink);
        learnBtn.read();
        extraBtn.read();
    }

    char mode() const
    {
        if (modeNbr == Single1Mode || modeNbr == Single2Mode)
        {
            return SingleMode;
        }
        if (modeNbr == Poly4Mode || modeNbr == Poly2Mode)
        {
            return PolyMode;
        }
        return MonoMode;
    }

    char grouping() const
    {
        if (modeNbr == Single2Mode || modeNbr == Mono2Mode || modeNbr == Poly2Mode)
        {
            return Grouping2;
        }
        if (modeNbr == Poly4Mode || modeNbr == Mono4Mode)
        {
            return Grouping4;
        }
        return Grouping1;
    }

    void setMode(char mode, char grouping)
    {
        if (grouping == Midi10UI::Grouping1)
        {
            modeNbr = Midi10UI::Single1Mode;
        }
        else if (grouping == Midi10UI::Grouping2)
        {
            if (mode == Midi10UI::SingleMode)
            {
                modeNbr = Midi10UI::Single2Mode;
            }
            else if (mode == Midi10UI::MonoMode)
            {
                modeNbr = Midi10UI::Mono2Mode;
            }
            else if (mode == Midi10UI::PolyMode)
            {
                modeNbr = Midi10UI::Poly2Mode;
            }
        }
        else if (grouping == Midi10UI::Grouping4)
        {
            if (mode == Midi10UI::MonoMode)
            {
                modeNbr = Midi10UI::Mono4Mode;
            }
            else if (mode == Midi10UI::PolyMode)
            {
                modeNbr = Midi10UI::Poly4Mode;
            }
        }
    }

    void showMode()
    {
        // show mode for a short time
        for (int idx = 0; idx < LedOutBank::Capacity; ++idx)
        {
            gateDigitalOut.set(idx, LedOutBank::Off);
        }
        gateDigitalOut.set(modeNbr, LedOutBank::On);
        gateDigitalOut.update(1);
        delay(200);
    }

    // void printToggleLearn(char mode, char grouping)
    // {
    //     if (debug)
    //     {
    //         Serial.print(mode);
    //         Serial.print(grouping);
    //         Serial.println(" toggle learn!");
    //     }
    // }
};
