#pragma once

#include <Arduino.h>
#include "analogoutbank.h"
#include "shiftoutbank.h"
#include "digitaloutbank.h"
#include "buttonin.h"

struct Midi8UI
{
    DigitalOutBank gatesOut;//(4,5,6,7);
    AnalogOutBank cvOut;//(4);// 2x DAC => use CS pins 10,9
    ShiftOutBank ledsOut;//(8);//use CS pin 8

    bool blink;
    //TODO button(s)
    ButtonIn m_LearnBtn;//3 

    Midi8UI()
    : gatesOut(4,5,6,7), cvOut(4), ledsOut(8), blink(false), m_LearnBtn()
    {}

    void begin()
    {
        gatesOut.begin();
        cvOut.begin();
        ledsOut.begin();
        m_LearnBtn.begin(3);//
    }

    void update()
    {
        gatesOut.update();
        cvOut.update();
        ledsOut.update();
        blink = millis() >> 6;//period 128 msec 
        m_LearnBtn.read();
    }
};
