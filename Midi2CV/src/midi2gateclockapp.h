#pragma once
#include <Arduino.h>

#include "midi2gate.h"
#include "midi2clock.h"
#include "analogbuttonin2.h"
#include "debugcounter.h"
#include "digitaloutbank.h"
#include "gateoutbank.h"
#include "ledout.h"


struct Midi2GateClockApp
{
  int statusLed1Pin;
  AnalogButtonIn2 buttons;
  DigitalOutBank digitalOutBank;
  GateOutBank gatesOut_midi2Gate;
  LedOut ledOut;
  GateOutBank gateOut_midi2Clock;
  LedOut clockLedOut;
  Midi2Gate midi2Gate;
  Midi2Clock midi2Clock;
  // mode : 0 = midi2gate 1=midi2clock
  int mode;

  void Begin(uint8_t analogBtnInPin, uint8_t statusLedPin,
              uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, 
              uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7)
  {
    buttons.Begin(analogBtnInPin);

    statusLed1Pin = statusLedPin;
    pinMode(statusLed1Pin, OUTPUT);
    ledOut.Begin();

    digitalOutBank.Begin(pin0, pin1, pin2, pin3, pin4, pin5, pin6, pin7);

    clockLedOut.Begin();
    gatesOut_midi2Gate.Begin();
    gateOut_midi2Clock.Begin();

    midi2Gate.Begin(&gatesOut_midi2Gate, &ledOut);
    midi2Clock.Begin(&gateOut_midi2Clock, &clockLedOut);
    
    mode = 0;
  }

  void OnMidiMessage(uint8_t byte)
  {
    midi2Clock.OnMessage(byte);
  }

  void OnMidiMessage(MidiVoiceMessage& message)
  {
    midi2Gate.OnMessage(message);
  }

  void Update(unsigned long millies)
  {
    // read and decode analogin from 2 buttons via R2R dac
    buttons.update();
    if (buttons.IsClicked1())// not in mode 0??
    {
      midi2Gate.ToggleLearning();
    }
    if(buttons.IsClicked2())
    {
      mode = 1- mode;
    }

    uint8_t counter = millies >> 2;
    gatesOut_midi2Gate.Update(millies);
    gateOut_midi2Clock.Update(millies);

    // apply depending on gate vs clock mode
    if(0 == mode)
    {
      gatesOut_midi2Gate.Apply(digitalOutBank);
      ledOut.Apply(counter, statusLed1Pin);
    }
    else
    {
      gateOut_midi2Clock.Apply(digitalOutBank);
      clockLedOut.Apply(counter, statusLed1Pin);
    }
  }
};