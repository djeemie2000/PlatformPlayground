#pragma once
#include <Arduino.h>

#include "mcpdac.h"
#include "fastdigitalwrite.h"
#include "analoginbank.h"
#include "fastdigitalinbank.h"
#include "fastdigitaloutbank.h"
#include "analoginbank821.h"
#include "shiftiobank.h"

// devBoard vs app => reuseable devBoard
struct DevBoard
{
    // MCP dac uses CS pin 10 
    MCPDac dac;                     // assumes arduino nano
    // 2x gate in pin D2 D3
    // 4x button in pin D6 D7 A4 A5 -> TODO debouncing
    FastDigitalInBank<6> gateButtonIn; // assumes arduino nano
    // 2x gate out pin D4 D5
    FastDigitalOutBank<2> gateOut; // assumes arduino nano
    // 8 to 3 analog in: Ain A3 + Din A0 A1 A2 = PORTC 0 1 2
    AnalogInBank821<0,1,2> analogIn; // assumes arduino nano
    // shift register in x1 + out x2 uses CS pin D9
    ShiftIOBank<16> shiftIO;

    void Begin()
    {
        dac.Begin();
        // gate in
        gateButtonIn.Assign(0, 2, true);
        gateButtonIn.Assign(1, 3, true);
        // button in
        gateButtonIn.Assign(2, 6, true);
        gateButtonIn.Assign(3, 7, true);
        gateButtonIn.Assign(4, A4, true);
        gateButtonIn.Assign(5, A5, true);
        // gate out
        gateOut.Assign(0, 4);
        gateOut.Assign(1, 5);
        // analog in 8 to 3 AIn pin A3 select pins A0 A1 A2
        analogIn.Begin(A3, 2);// change threshold > 2
        // shift register IO CS pin 9
        shiftIO.Begin(9);
    }
};

