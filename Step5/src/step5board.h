#pragma once
#include <Arduino.h>

//#define FAKECLOCK 1

#include "digitaloutbank.h"
#include "ledoutbank.h"
#include "buttoninbank.h"
#include "digitalinbank.h"
#include "analogin.h"
//#include "EEPROM.h"

// const int ClockInPin = 2;
// const int ResetInPin = 3;

struct Step5Board
{
    DigitalOutBank10 stepOut; 
    DigitalOutBank selectGateOut;
    DigitalInBank clockResetIn;
    ButtonInBank resetBtnIn;
    //TODO 3x analog in CV bus 1 2 pot length
    AnalogIn lengthIn;
    AnalogIn bus1In;
    AnalogIn bus2In;

  Step5Board() : stepOut(), selectGateOut(), clockResetIn(), resetBtnIn()
  {

  }

  void Begin()
  {
    stepOut.Begin(4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
    selectGateOut.Begin(A0, A1, A2, A3);
    clockResetIn.Begin(2, 3);
    resetBtnIn.Begin(A5);
    lengthIn.Begin(A4);
    bus1In.Begin(A6);
    bus2In.Begin(A7);
  }
  
};
