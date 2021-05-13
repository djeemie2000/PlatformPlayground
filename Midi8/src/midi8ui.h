#pragma once

#include <Arduino.h>
#include "analogoutbank.h"
#include "ledoutbank.h"
#include "DigitalOutBank.h"
#include "buttonin.h"
#include "select.h"

struct Midi8UI
{
    static const char SingleMode = 'S';
    static const char MonoMode = 'M';
    static const char PolyMode = 'P';

    static const char Grouping1 = '1';
    static const char Grouping2 = '2';
    static const char Grouping4 = '4';

    DigitalOutBank gatesOut; //(4,5,6,7);
    AnalogOutBank cvOut;     //(4);// 2x DAC => use CS pins 10,9
    LedOutBank ledsOut;      //(8);//use CS pin 8

    // toggle learn on/off
    ButtonIn learnBtn; // pin 2
    ButtonIn extraBtn; // pin 3

    // grouping (1/2/4) + mode (single/mono/poly)
    char grouping;
    char mode;

    ButtonIn modeBtn1; // pin A0
    ButtonIn modeBtn2; // pin A1
    ButtonIn modeBtn3; // pin A2
    ButtonIn modeBtn4; // pin A3
    ButtonIn modeBtn5; // pin A4
    ButtonIn modeBtn6; // pin A5
    //TODO analogRead ==0 or not // pin A6
    //TODO analogRead ==0 or not // pin A7

    bool debug;

    static const int NoLearn = 0;
    static const int Learn1 = 1;
    static const int Learn2 = 2;
    Select learnMode;

    Midi8UI()
        : gatesOut(), cvOut(4), ledsOut(8), learnBtn(), debug(true)
    {
    }

    void begin()
    {
        gatesOut.begin(4, 5, 6, 7);
        cvOut.begin();
        ledsOut.begin();
        const int debounce = 30;
        learnBtn.begin(2, debounce);
        extraBtn.begin(3, debounce);
        modeBtn1.begin(A0, debounce);
        modeBtn2.begin(A1, debounce);
        modeBtn3.begin(A2, debounce);
        modeBtn4.begin(A3, debounce);
        modeBtn5.begin(A4, debounce);
        modeBtn6.begin(A5, debounce);

        grouping = Grouping1;
        mode = SingleMode;
    }

    void update()
    {
        gatesOut.update();
        cvOut.update();
        int blink = (millis() >> 7) & 1; //period 256 msec
        ledsOut.update(blink);
        learnBtn.read();
        extraBtn.read();
        modeBtn1.read();
        modeBtn2.read();
        modeBtn3.read();
        modeBtn4.read();
        modeBtn5.read();
        modeBtn6.read();

        if (modeBtn1.Get())
        {
            grouping = Grouping1;
            mode = SingleMode;
        }
        else if (modeBtn2.Get())
        {
            grouping = Grouping2;
            mode = SingleMode;
        }
        else if (modeBtn3.Get())
        {
            grouping = Grouping2;
            mode = MonoMode;
        }
        else if (modeBtn4.Get())
        {
            grouping = Grouping2;
            mode = PolyMode;
        }
        else if (modeBtn5.Get())
        {
            grouping = Grouping4;
            mode = MonoMode;
        }
        else if (modeBtn6.Get())
        {
            grouping = Grouping4;
            mode = PolyMode;
        }
    }
};
