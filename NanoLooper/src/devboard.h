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
    // 8x pots
    // 8 to 3 analog in: Ain A3 + Din A0 A1 A2 = PORTC 0 1 2
    AnalogInBank821<0,1,2> potIn; // assumes arduino nano
    // 8x patch in
    // 8x patch out 
    // 8x leds out
    // shift register in x1 + out x2 uses CS pin D9
    ShiftIOBank<16> shiftIO;
    // audio in left/right
    AnalogInBank<2> audioIn;

    void Begin()
    {
        dac.Begin();
        // gate in -> pullup
        gateButtonIn.Assign(0, 2, true);
        gateButtonIn.Assign(1, 3, true);
        // button in -> no pullup
        gateButtonIn.Assign(2, 6, false);
        gateButtonIn.Assign(3, 7, false);
        gateButtonIn.Assign(4, A4, false);
        gateButtonIn.Assign(5, A5, false);
        // gate out
        gateOut.Assign(0, 4);
        gateOut.Assign(1, 5);
        // analog in 8 to 3 AIn pin A3 select pins A0 A1 A2
        potIn.Begin(A3, 2);// change threshold > 2
        // shift register IO 
        // CS pin 9
        // shift register in load pin 8
        shiftIO.Begin(9, 8);
        // 
        audioIn.Begin();
        audioIn.Assign(0, A6);
        audioIn.Assign(1, A7);
    }

    // TODO functions for gate out, button in, patch out, patch in, led out, audio in (int vs 8 bit?)
    // TODO update functions for each IO (?)
    void SetPatchOut(int idx, int value)
    {
        shiftIO.Set(idx, value);
    }

    int GetPatchIn(int idx) const
    {
        return shiftIO.Get(8+idx);
    }

    int GetPatchInRising(int idx) const
    {
        return shiftIO.IsRising(8+idx);
    }

    void SetLedOut(int idx, int value)
    {
        //TODO dirty hack to fix HW issue by remapping idx 0 1 2 7 3 4 5 6 
        shiftIO.Set(idx+8, value);
    }

    int GetButton(int idx) const
    {
        // no pullup no invert
        return gateButtonIn.Get(2+idx);
    }

    int GetButtonPressed(int idx) const
    {
        // no pullup no invert
        return gateButtonIn.IsRising(2+idx);
    }

    void SetGateOut(int idx, int value)
    {
        if(value)
        {
            gateOut.Set(idx);
        }
        else
        {
            gateOut.Clear(idx);
        }
    }

    int GetGateIn(int idx) const
    {
        return 1-gateButtonIn.Get(idx);
    }

    int GetGateInRising(int idx) const
    {
        return gateButtonIn.IsFalling(idx);
    }

    int GetGateInFalling(int idx) const
    {
        return gateButtonIn.IsRising(idx);
    }

    int GetPot(int idx) const
    {
        return potIn.Get(idx);
    }

    bool GetPotChanged(int idx) const
    {
        return potIn.IsChanged(idx);
    }

    uint8_t GetAudioInL8bit()
    {
        return audioIn.Get(0)>>2;
    }

    uint8_t GetAudioInR8bit()
    {
        return audioIn.Get(1)>>2;
    }

    
};

