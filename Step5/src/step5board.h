#pragma once
#include <Arduino.h>

//#define FAKECLOCK 1

#include "digitaloutbank.h"
#include "fastdigitaloutbank.h"
#include "ledoutbank.h"
#include "buttoninbank.h"
#include "digitalinbank.h"
#include "analogin.h"
//#include "EEPROM.h"
#include "fakeclock.h"

// const int ClockInPin = 2;
// const int ResetInPin = 3;

struct Step5Board
{
    //DigitalOutBank10 stepOut; 
    //DigitalOutBank selectGateOut;

    FastDigitalOutBank<12> stepSelectOut;
    FastDigitalOutBank<2> gateOut;

    DigitalInBank clockResetIn;
    ButtonInBank resetBtnIn;
    //3x analog in CV bus 1 2 pot length
    AnalogIn lengthIn;
    AnalogIn bus1In;
    AnalogIn bus2In;
    // 
    FakeClock fakeClock;

  Step5Board() : stepSelectOut(), gateOut(), clockResetIn(), resetBtnIn()
  {

  }

  void Begin()
  {
    pinMode(LED_BUILTIN, OUTPUT);
    //stepOut.Begin(4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
    //selectGateOut.Begin(A0, A1, A2, A3);//select 1 select 2 gate out 1 gate out 2

    stepSelectOut.Begin();
    // step out
    stepSelectOut.Assign(0, 4);
    stepSelectOut.Assign(1, 5);
    stepSelectOut.Assign(2, 6);
    stepSelectOut.Assign(3, 7);
    stepSelectOut.Assign(4, 8);
    stepSelectOut.Assign(5, 9);
    stepSelectOut.Assign(6, 10);
    stepSelectOut.Assign(7, 11);
    stepSelectOut.Assign(8, 12);
    stepSelectOut.Assign(9, 13);
    //select 1 select 2 
    stepSelectOut.Assign(10, A0);
    stepSelectOut.Assign(11, A1);
    
    gateOut.Begin();
    // gate out 1 gate out 2
    gateOut.Assign(0, A2);
    gateOut.Assign(1, A3);

    clockResetIn.Begin(2, 3);
    resetBtnIn.Begin(A5);
    lengthIn.Begin(A4);
    bus1In.Begin(A6);
    bus2In.Begin(A7);
    fakeClock.Begin(300);
  }

  void LedOn()
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  void LedOff()
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  
};
