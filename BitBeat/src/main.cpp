#include <Arduino.h>

#include "digitaloutbank.h"
#include "ledoutbank.h"
#include "buttoninbank.h"


const int ClockInPin = 2;
const int ResetInPin = 3;

const int Btn0Pin = 4;
const int Btn50Pin = 5;
const int Btn100Pin = 6;
const int BtnResetPin = 7;

const int Gate1Pin = 8;
const int Led1Pin = 9;
const int Btn1Pin = 10;

const int Gate2Pin = 11;
const int Led2Pin = 12;
const int Btn2Pin = 13;// led pin -> cannot be used as input!!!!!!!!!!!!

const int Gate3Pin = A0;
const int Led3Pin = A1;
const int Btn3Pin = A2;

const int Gate4Pin = A3;
const int Led4Pin = A4;
const int Btn4Pin = A5;

// TODO led out bank(s), on/off/blink
// TODO gateInBank
// TODO BtnInBank, incl debounce

// TODO test leds out
// TODO test gate in (rising, falling)
// TODO test Btn in (pressed, released)

struct BitBeatApp
{
  // tracks
  DigitalOutBank m_GateOut;
  LedOutBank m_LedOut;
  ButtonInBank m_ButtonIn;

  ButtonInBank m_FunctionButtonIn;

  BitBeatApp()
  {}

  void Begin()
  {
    m_GateOut.Begin(Gate1Pin, Gate2Pin, Gate3Pin, Gate4Pin);
    m_LedOut.Begin(Led1Pin, Led2Pin, Led3Pin, Led4Pin);
    m_ButtonIn.Begin(Btn1Pin, Btn2Pin, Btn3Pin, Btn4Pin);

    m_FunctionButtonIn.Begin(Btn0Pin, Btn50Pin, Btn100Pin, BtnResetPin);
  }

};

BitBeatApp app;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("BitBeat v0.1...");

  app.Begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  //TestDigitalOutBank(app.m_GateOut, 2);
  //TestLedOut(app.m_LedOut, 1);
  //TestButtonInGet(app.m_ButtonIn, 1);
  //TestButtonIn(app.m_ButtonIn, 2);
  TestButtonIn(app.m_FunctionButtonIn, 2);
}
