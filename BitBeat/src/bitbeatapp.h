#pragma once
#include <Arduino.h>

//#define FAKECLOCK 1

#include "digitaloutbank.h"
#include "ledoutbank.h"
#include "buttoninbank.h"
#include "digitalinbank.h"
#include "bitbeattrack.h"
#include "EEPROM.h"

const int ClockInPin = 2;
const int ResetInPin = 3;

const int Btn0Pin = 4;
const int Btn50Pin = 5;
const int Btn100Pin = 6;
const int BtnResetPin = 7;

const int Gate1Pin = 10; 
const int Led1Pin = 9;
const int Btn1Pin = 8;

const int Gate2Pin = 13;
const int Led2Pin = 12;
const int Btn2Pin = 11; // led pin -> cannot be used as input!!!!!!!!!!!!

const int Gate3Pin = A2;
const int Led3Pin = A1;
const int Btn3Pin = A0;

const int Gate4Pin = A5;
const int Led4Pin = A4;
const int Btn4Pin = A3;

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

    static const int NumSlots = 4;
    uint8_t m_Slot;

#ifdef FAKECLOCK
    // fake clock
    unsigned long m_Millies;
    bool m_Clock;
#endif

    BitBeatApp() : m_RecordingTrack(-1), m_Slot(0)
#ifdef FAKECLOCK
    , m_Millies(0), m_Clock(false)
#endif
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

        // mode precedence:
        // 1) recording
        // 2) slots     : no recording mode for any track AND btn 0% is pressed
        // 3) play/mute : no recording mode for any track AND btn 50% is pressed
        bool recordingMode = (0<=m_RecordingTrack);
        bool slotMode = !recordingMode && m_FunctionButtonIn.Get(0);
        bool playMuteMode = !recordingMode && ! playMuteMode && m_FunctionButtonIn.Get(1);

        // tracks button input 
        for (int tr = 0; tr < NumTracks; ++tr)
        {
            //handle track button input for recording mode
            if(!playMuteMode && !slotMode)
            {
                if (m_ButtonIn.IsClicked(tr))
                {
                    // toggle start/stop recording
                    if (tr == m_RecordingTrack)
                    {
                        m_Track[tr].StopRecording();
                        SaveTrackParams(tr);
                        m_RecordingTrack = -1;
                        // Serial.print(tr);
                        // Serial.println(" stop rec");
                    }
                    else
                    {
                        // only one recording track at a time!
                        if (0 <= m_RecordingTrack)
                        {
                            m_Track[m_RecordingTrack].StopRecording();
                            SaveTrackParams(m_RecordingTrack);
                            // Serial.print(m_RecordingTrack);
                            // Serial.println(" stop rec");
                        }
                        m_RecordingTrack = tr;
                        m_Track[tr].StartRecording();
                        // Serial.print(tr);
                        // Serial.println(" start rec");
                    }
                }
            }

            // track output
            if (tr == m_RecordingTrack)
            {
                m_Track[tr].Record(m_FunctionButtonIn.IsClicked(0), m_FunctionButtonIn.IsClicked(1), m_FunctionButtonIn.IsClicked(2));
                m_Track[tr].DisplayRecording(m_GateOut, m_LedOut, tr);
            }
            else
            {
                // play mode or play + play mute mode
                m_Track[tr].Play(clockRising, clockFalling, m_GateIn.IsClicked(1), m_FunctionButtonIn.IsClicked(3), 
                                    playMuteMode, m_ButtonIn.IsClicked(tr));
                m_Track[tr].DisplayPlay(m_GateOut, tr);
                if(playMuteMode)
                {
                    m_Track[tr].DisplayPlayMute(m_LedOut, tr);
                }
                else if(!slotMode)
                {
                    m_Track[tr].DisplayPlay(m_LedOut, tr);
                }
            }

            if(slotMode)
            {
                // if any track button is clicked in slot mode, change slot, then save slot, then load params from slot
                for(int tr = 0; tr<NumTracks; ++tr)
                {
                    if(m_ButtonIn.IsClicked(tr))
                    {
                        m_Slot = tr;
                        SaveGlobalHeader();//also saves m_Slot!
                        LoadParams();//load params 
                        break;
                    }   
                }
                // display current slot in slot mode
                for(int tr = 0; tr<NumTracks; ++tr)
                {
                    m_LedOut.LedOff(tr);
                }
                m_LedOut.LedOn(m_Slot);
            }
        }

        // update leds
        uint8_t counter = millies >> 2;
        m_LedOut.Update(counter);
    }

    int TrackParamOffset(int tr)
    {
        return 4+4+(m_Slot*NumTracks*BitBeatTrack::ParamSize())+(tr*BitBeatTrack::ParamSize());
    }

    void SaveGlobalHeader()
    {
        int off = 0;
        // header BitBeat BB
        EEPROM.update(off++, 'B');
        EEPROM.update(off++, 'B');
        // version V0.1
        EEPROM.update(off++, 0x00);
        EEPROM.update(off++, 0x02);
        // global params, slot, reserved
        EEPROM.update(off++, 'B');
        EEPROM.update(off++, 'G');
        EEPROM.update(off++, m_Slot);
        EEPROM.update(off++, 0x00);    
    }

    void SaveTrackParams(int tr)
    {
        // always update header!
        SaveGlobalHeader();
        // track
        m_Track[tr].SaveParams(TrackParamOffset(tr));
    }

    void SaveParams()
    {
        SaveGlobalHeader();     
        // tracks
        for(int tr = 0; tr<NumTracks; ++ tr)
        {
            m_Track[tr].SaveParams(TrackParamOffset(tr));
        }
    }

    void LoadParams()
    {
        int off = 0;
        if ('B' == EEPROM.read(off++) && 'B' == EEPROM.read(off++))
        {
            // only read V0.2 params
            if(0x00 == EEPROM.read(off++) && 0x02 == EEPROM.read(off++))
            {
                if('B' == EEPROM.read(off++) && 'G' == EEPROM.read(off++))
                {
                    // global params
                    uint8_t slot = EEPROM.read(off++);
                    EEPROM.read(off++);//reserved

                    if(slot<NumSlots)
                    {
                        m_Slot = slot;
                    }
                }

                // tracks
                for(int tr = 0; tr<NumTracks; ++ tr)
                {
                    m_Track[tr].LoadParams(TrackParamOffset(tr));
                }
            }
        }
    }

};
