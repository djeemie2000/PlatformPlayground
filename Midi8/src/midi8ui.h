#pragma once

#include <Arduino.h>
#include "analogoutbank.h"
#include "ledoutbank.h"
#include "digitaloutbank.h"
#include "buttonin.h"

struct Midi8UI
{
    DigitalOutBank gatesOut;//(4,5,6,7);
    AnalogOutBank cvOut;//(4);// 2x DAC => use CS pins 10,9
    LedOutBank ledsOut;//(8);//use CS pin 8

    // toggle learn on/off
    ButtonIn learnBtn;//2 ipv 3 
    // toggle mode 1/2/4
    //TODO toggle mode button
    int mode;

    Midi8UI()
    : gatesOut(4,5,6,7), cvOut(4), ledsOut(8), learnBtn()
    {}

    void begin()
    {
        gatesOut.begin();
        cvOut.begin();
        ledsOut.begin();
        learnBtn.begin(2,30);//

        mode = 1;
    }

    void update()
    {
        gatesOut.update();
        cvOut.update();
        int blink = (millis() >> 5) & 1;//period 256 msec 
        ledsOut.update(blink);
        learnBtn.read();
    }
};
