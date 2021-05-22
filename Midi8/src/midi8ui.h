#pragma once

#include <Arduino.h>
#include "analogoutbank.h"
#include "ledoutbank.h"
#include "DigitalOutBank.h"
#include "buttonin.h"

struct Midi8UI
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
    static const int Learn1 = 1;
    static const int Learn2 = 2;
    int learnMode;

    DigitalOutBank gatesOut; //(4,5,6,7);
    AnalogOutBank cvOut;     //(4);// 2x DAC => use CS pins 10,9
    LedOutBank ledsOut;      //(8);//use CS pin 8

    // toggle learn on/off
    ButtonIn learnBtn; // pin 2
    ButtonIn extraBtn; // pin 3

    //TODO analogRead ==0 or not // pin A6
    //TODO analogRead ==0 or not // pin A7

    Midi8UI()
        : modeNbr(Single1Mode), debug(true), learnMode(0), gatesOut(), cvOut(), ledsOut(8), learnBtn(), extraBtn()
    {
    }

    void begin()
    {
        modeNbr = Single1Mode;
        learnMode = NoLearn;

        gatesOut.begin(4, 5, 6, 7, A0, A1, A2, A3, A4, A5);
        cvOut.begin(4);
        ledsOut.begin();
        const int debounce = 30;
        learnBtn.begin(2, debounce);
        extraBtn.begin(3, debounce);
    }

    void update()
    {
        gatesOut.update();
        cvOut.update();
        int blink = (millis() >> 7) & 1; //period 256 msec
        ledsOut.update(blink);
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
        if (grouping == Midi8UI::Grouping1)
        {
            modeNbr = Midi8UI::Single1Mode;
        }
        else if (grouping == Midi8UI::Grouping2)
        {
            if (mode == Midi8UI::SingleMode)
            {
                modeNbr = Midi8UI::Single2Mode;
            }
            else if (mode == Midi8UI::MonoMode)
            {
                modeNbr = Midi8UI::Mono2Mode;
            }
            else if (mode == Midi8UI::PolyMode)
            {
                modeNbr = Midi8UI::Poly2Mode;
            }
        }
        else if (grouping == Midi8UI::Grouping4)
        {
            if (mode == Midi8UI::MonoMode)
            {
                modeNbr = Midi8UI::Mono4Mode;
            }
            else if (mode == Midi8UI::PolyMode)
            {
                modeNbr = Midi8UI::Poly4Mode;
            }
        }
    }

    void showMode()
    {
        // show mode for a short time
        for (int idx = 0; idx < LedOutBank::Capacity; ++idx)
        {
            ledsOut.set(idx, LedOutBank::Off);
        }
        ledsOut.set(modeNbr, LedOutBank::On);
        ledsOut.update(1);
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
