#pragma once
#include <Arduino.h>
#include "digitaloutbank.h"
#include "buttoninbank.h"
#include "rgled.h"
#include "pitchin.h"
#include "mcp4822bank.h"
#include "pitchoutbank.h"

struct QuantizerBoard
{
    DigitalOutBank triggerOut;//TODO trigger/gate out
    ButtonInBank buttons;
    //TODO bicolor led x2 -> blink support?
    RGLed led1Out;//
    RGLed led2Out;//
    PitchIn pitchIn1;
    PitchIn pitchIn2;
    MCP4822Bank analogOut;
    PitchOutBank pitchOut;

    QuantizerBoard() : triggerOut(), buttons()
    {}

    void Begin()
    {
        triggerOut.Begin(4,7);
        buttons.Begin(A0, A1, A2, A3);
        led1Out.Begin(6, 9);
        led2Out.Begin(3, 5);
        pitchIn1.Begin(A6);
        pitchIn2.Begin(A7);
        analogOut.Begin();
        pitchOut.Begin(&analogOut);
    }
};
