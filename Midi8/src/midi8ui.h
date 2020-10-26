#pragma once

#include <Arduino.h>
#include "analogoutbank.h"
#include "ledoutbank.h"
#include "digitaloutbank.h"
#include "buttonin.h"

struct Midi8UI
{
    static const int SingleMode = 0;
    static const int MonoMode = 1;
    static const int PolyMode = 2;

    static const int Grouping1 = 3;
    static const int Grouping2 = 4;
    static const int Grouping4 = 5;

    DigitalOutBank gatesOut;//(4,5,6,7);
    AnalogOutBank cvOut;//(4);// 2x DAC => use CS pins 10,9
    LedOutBank ledsOut;//(8);//use CS pin 8

    // toggle learn on/off
    ButtonIn learnBtn;// pin 2 
    
    // grouping 1/2/4
    ButtonIn groupingBtn1;// pin A0
    ButtonIn groupingBtn2;// pin A1
    int grouping;

    // mode Single/Mono/Poly
    ButtonIn modeBtn1;// pin A2
    ButtonIn modeBtn2;// pin A3
    int mode;


    Midi8UI()
    : gatesOut(), cvOut(4), ledsOut(8), learnBtn()
    {}

    void begin()
    {
        gatesOut.begin(4,5,6,7);
        cvOut.begin();
        ledsOut.begin();
        const int debounce = 30;
        learnBtn.begin(2,debounce);
        groupingBtn1.begin(A0,30);
        groupingBtn2.begin(A1,30);
        modeBtn1.begin(A2,30);
        modeBtn2.begin(A3,30);
        
        grouping = Grouping1;
        mode = SingleMode;
    }

    void update()
    {
        gatesOut.update();
        cvOut.update();
        int blink = (millis() >> 7) & 1;//period 256 msec 
        ledsOut.update(blink);
        learnBtn.read();
        groupingBtn1.read();
        groupingBtn2.read();
        modeBtn1.read();
        modeBtn2.read();

        if(groupingBtn1.Get())
        {
            grouping = Grouping1;
        }
        else if(groupingBtn2.Get())
        {
            grouping = Grouping2;
        }
        else
        {
            grouping = Grouping4;
        }
        
        if(modeBtn1.Get())
        {
            mode = SingleMode;
        }
        else if(modeBtn2.Get())
        {
            mode = MonoMode;
        }
        else
        {
            mode = PolyMode;
        }
    }
};
