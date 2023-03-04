#include <Arduino.h>

#include "digitaloutbank.h"
#include "ledoutbank.h"
#include "buttoninbank.h"
#include "bitbeattrack.h"


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
  DigitalOutBank m_GateOut;
  LedOutBank m_LedOut;
  ButtonInBank m_ButtonIn;
  ButtonInBank m_FunctionButtonIn;
  // TODO digitalinbank clock reset

  static const int NumTracks = 4;
  BitBeatTrack m_Track[NumTracks];
  int m_RecordingTrack;

  // fake clock
  unsigned long m_Millies;
  bool m_Clock;

  BitBeatApp() : m_RecordingTrack(-1), m_Millies(0), m_Clock(false)
  {}

  void Begin()
  {
    m_GateOut.Begin(Gate1Pin, Gate2Pin, Gate3Pin, Gate4Pin);
    m_LedOut.Begin(Led1Pin, Led2Pin, Led3Pin, Led4Pin);
    m_ButtonIn.Begin(Btn1Pin, Btn2Pin, Btn3Pin, Btn4Pin);//hack to avoid led pin as input
    m_FunctionButtonIn.Begin(Btn0Pin, Btn50Pin, Btn100Pin, BtnResetPin);
  }

  void Update()
  {
    unsigned long millies = millis();
    m_ButtonIn.Update(millies);
    m_FunctionButtonIn.Update(millies);

    //TODO fake clock
    bool clockRising = false;
    bool clockFalling = false;
    unsigned long elapsed = millies - m_Millies;
    if(125<elapsed)
    {
      m_Millies = millies;
      m_Clock = !m_Clock;
      if(m_Clock)
      {
        clockRising = true;
      }
      else
      {
        clockFalling = true;
      }
    }

    for(int tr = 0; tr<NumTracks; ++ tr)
    {
      if(tr==1)
      {
        continue;//dirty hack!
      }

      if(m_ButtonIn.IsClicked(tr))
      {
        // toggle start/stop recording
        if(tr == m_RecordingTrack)
        {
          m_Track[tr].StopRecording(m_GateOut, m_LedOut, tr);
          m_RecordingTrack = -1;

          Serial.print(tr);
          Serial.println(" stop rec");
        }
        else
        {
          // only one recording track at a time!
          if(0<=m_RecordingTrack)
          {
            m_Track[m_RecordingTrack].StopRecording(m_GateOut, m_LedOut, m_RecordingTrack);

            Serial.print(m_RecordingTrack);
            Serial.println(" stop rec");
          }
          m_RecordingTrack = tr;
          m_Track[tr].StartRecording(m_GateOut, m_LedOut, tr);

          Serial.print(tr);
          Serial.println(" start rec");
        }
      }

      if(tr == m_RecordingTrack)
      {
        m_Track[tr].Record(m_FunctionButtonIn.IsClicked(0), m_FunctionButtonIn.IsClicked(1), m_FunctionButtonIn.IsClicked(2));
      }
      else
      {
        m_Track[tr].Play(clockRising, clockFalling, false, m_FunctionButtonIn.IsClicked(3), m_GateOut, m_LedOut, tr);
      }
    }

    uint8_t counter = millies >> 2;
    m_LedOut.Update(counter);
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
  //TestButtonIn(app.m_FunctionButtonIn, 4);

  while(true)
  {
    app.Update();
  }
}
