#pragma once
#include <Arduino.h>

#define FAKECLOCK 1

#include "digitaloutbank.h"
#include "ledoutbank.h"
#include "buttoninbank.h"
#include "digitalinbank.h"
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
const int Led2Pin = 13;
const int Btn2Pin = 12; // led pin -> cannot be used as input!!!!!!!!!!!!

const int Gate3Pin = A0;
const int Led3Pin = A1;
const int Btn3Pin = A2;

const int Gate4Pin = A3;
const int Led4Pin = A4;
const int Btn4Pin = A5;

struct BitBeatApp
{
    DigitalOutBank m_GateOut;
    LedOutBank m_LedOut;
    ButtonInBank m_ButtonIn;
    ButtonInBank m_FunctionButtonIn;
    DigitalInBank m_GateIn; // clock reset

    static const int NumTracks = 4;
    BitBeatTrack m_Track[NumTracks];
    int m_RecordingTrack;

#ifdef FAKECLOCK
    // fake clock
    unsigned long m_Millies;
    bool m_Clock;
#endif

    BitBeatApp() : m_RecordingTrack(-1), m_Millies(0), m_Clock(false)
    {
    }

    void Begin()
    {
        m_GateOut.Begin(Gate1Pin, Gate2Pin, Gate3Pin, Gate4Pin);
        m_LedOut.Begin(Led1Pin, Led2Pin, Led3Pin, Led4Pin);
        m_ButtonIn.Begin(Btn1Pin, Btn2Pin, Btn3Pin, Btn4Pin);
        m_FunctionButtonIn.Begin(Btn0Pin, Btn50Pin, Btn100Pin, BtnResetPin);
        m_GateIn.Begin(ClockInPin, ResetInPin);
    }

    void Update()
    {
        // read inputs
        unsigned long millies = millis();
        m_ButtonIn.Update(millies);
        m_FunctionButtonIn.Update(millies);
        m_GateIn.Update();

        // clock
        bool clockRising = false;
        bool clockFalling = false;
#ifdef FAKECLOCK
        // fake clock
        unsigned long elapsed = millies - m_Millies;
        if (125 < elapsed)
        {
            m_Millies = millies;
            m_Clock = !m_Clock;
            if (m_Clock)
            {
                clockRising = true;
            }
            else
            {
                clockFalling = true;
            }
        }
#endif

#ifndef FAKECLOCK
        clockRising = m_GateIn.IsClicked(0);
        clockFalling = m_GateIn.IsReleased(0);
#endif

        // tracks
        for (int tr = 0; tr < NumTracks; ++tr)
        {
            // if (tr == 1)
            // {
            //     continue; // dirty hack beacuse pin 13 is led pin so no input is possible!
            // }

            //handle track button input
            if (m_ButtonIn.IsClicked(tr))
            {
                // toggle start/stop recording
                if (tr == m_RecordingTrack)
                {
                    m_Track[tr].StopRecording(m_GateOut, m_LedOut, tr);
                    m_RecordingTrack = -1;
                    // Serial.print(tr);
                    // Serial.println(" stop rec");
                }
                else
                {
                    // only one recording track at a time!
                    if (0 <= m_RecordingTrack)
                    {
                        m_Track[m_RecordingTrack].StopRecording(m_GateOut, m_LedOut, m_RecordingTrack);
                        // Serial.print(m_RecordingTrack);
                        // Serial.println(" stop rec");
                    }
                    m_RecordingTrack = tr;
                    m_Track[tr].StartRecording(m_GateOut, m_LedOut, tr);
                    // Serial.print(tr);
                    // Serial.println(" start rec");
                }
            }

            // track output
            if (tr == m_RecordingTrack)
            {
                m_Track[tr].Record(m_FunctionButtonIn.IsClicked(0), m_FunctionButtonIn.IsClicked(1), m_FunctionButtonIn.IsClicked(2));
            }
            else
            {
                m_Track[tr].Play(clockRising, clockFalling, m_GateIn.IsClicked(1), m_FunctionButtonIn.IsClicked(3), m_GateOut, m_LedOut, tr);
            }
        }

        // update leds
        uint8_t counter = millies >> 2;
        m_LedOut.Update(counter);
    }
};
