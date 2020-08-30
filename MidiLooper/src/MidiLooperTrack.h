#pragma once
#include <Arduino.h>
#include "MidiOut.h"
#include "MidiNoteStack.h"
#include "MidiLooperTicker.h"

struct MidiLooperItem
{
    uint8_t m_Step;
    uint8_t m_MidiNote;
    uint8_t m_Velocity; // 0x00 means note off
};

class MidiLooperTrack
{
public:
    MidiLooperTrack()
        : m_MidiChannel(0), m_MidiLearn(true), m_Recording(false), m_Muted(false), m_NumItems(0)
    {
    }

    void onUndo(MidiOut &midiOut)
    {
        //for now, clear all items
        // TODO only undo last recorded 'layer'

        // all playing notes should be note off!! => note stack
        AllNotesOff(midiOut);
        // TODO all 'cleared' note off events should be sent right now!
        // TODO only those after current step??
        // int idx = 0;
        // while (idx < m_NumItems)
        // {
        //     if (!m_Items[idx].m_Velocity)
        //     {
        //         // use some dummy velocity but not zero!
        //         midiOut.NoteOff(m_MidiChannel, m_Items[idx].m_MidiNote, 0x01);
        //     }
        //     ++idx;
        // }

        m_NumItems = 0;
    }

    void onToggleMuted(MidiOut &midiOut)
    {
        m_Muted = !m_Muted;
        //TODO all notes off ~ steps??
        if (m_Muted)
        {
            midiOut.allNotesOff(m_MidiChannel);
        }
    }

    void ToggleRecording()
    {
        m_Recording = !m_Recording;
    }

    void StartRecording()
    {
        m_Recording = true;
    }

    void StopRecording()
    {
        m_Recording = false;
    }

    void StartMidiLearn()
    {
        m_MidiLearn = true;
    }

    void onTick(const MidiLooperTicker &ticker, MidiOut &midiOut)
    {
        if (ticker.clockIsRising())
        {
            //TODO if muted => all note off?
            //TODO if channel changed (midi learn) => all notes off on original channel?
            int step = ticker.Counter(1); //TODO divider

            // iterate all items, if step matches current step, play it
            // muted => no note on, but still do all note off (?)
            if (!m_Muted)
            {
                int idx = 0;
                while (idx < m_NumItems)
                {
                    if (step == m_Items[idx].m_Step)
                    {
                        if (m_Items[idx].m_Velocity)
                        {
                            midiOut.NoteOn(m_MidiChannel, m_Items[idx].m_MidiNote, m_Items[idx].m_Velocity);
                            m_NoteStack.NoteOn(m_Items[idx].m_MidiNote);
                        }
                        else
                        {
                            // use some dummy velocity but not zero!
                            midiOut.NoteOff(m_MidiChannel, m_Items[idx].m_MidiNote, 0x01);
                            m_NoteStack.NoteOff(m_Items[idx].m_MidiNote);
                        }
                    }
                    ++idx;
                }
            }
        }
    }

    void onNoteOn(const MidiLooperTicker &ticker, MidiOut &midiOut, uint8_t midiChannel, uint8_t midiNote, uint8_t velocity)
    {
        //TODO if midi learn changes channels, need to note off all open notes!
        //TODO midi learn also learns 2 cc sliders!
        if (m_MidiLearn)
        {
            midiOut.allNotesOff(m_MidiChannel);
            //TODO only notes of this track not all notes of this channel!!
            m_MidiChannel = midiChannel;
            m_MidiLearn = false;
        }
        if (midiChannel == m_MidiChannel)
        {
            if (m_Recording && m_NumItems < ItemCapacity)
            {
                m_Items[m_NumItems].m_MidiNote = midiNote;
                m_Items[m_NumItems].m_Velocity = velocity;

                m_Items[m_NumItems].m_Step = ticker.recordingStep(1);
                ++m_NumItems;
            }
        }
    }

    void onNoteOff(const MidiLooperTicker &ticker, uint8_t midiChannel, uint8_t midiNote, uint8_t velocity)
    {
        if (midiChannel == m_MidiChannel)
        {
            if (m_Recording && m_NumItems < ItemCapacity)
            {
                m_Items[m_NumItems].m_MidiNote = midiNote;
                m_Items[m_NumItems].m_Velocity = 0x00; //note off <=> item velocity zero
                m_Items[m_NumItems].m_Step = ticker.recordingStep(1);
                ++m_NumItems;
            }
        }
    }

    void AllNotesOff(MidiOut &midiOut)
    {
        //midiOut.allNotesOff(m_MidiChannel);
        while (m_NoteStack.Size())
        {
            // fixed non zero velocity
            midiOut.NoteOff(m_MidiChannel, m_NoteStack.Pop(), 0x01);
        }
    }

    void printItems(HardwareSerial &serial)
    {
        serial.print("items # ");
        serial.println(m_NumItems);

        for (int idx = 0; idx < m_NumItems; ++idx)
        {
            serial.print(m_Items[idx].m_Step);
            serial.print(" ");
            serial.print(m_Items[idx].m_MidiNote, HEX);
            serial.print(" ");
            serial.println(m_Items[idx].m_Velocity, HEX);
        }
    }

    void printState(HardwareSerial &serial)
    {
        serial.print("Ch");
        serial.print(m_MidiChannel, HEX);
        serial.print(" L");
        serial.print(m_MidiLearn ? 1 : 0);
        serial.print(" R");
        serial.print(m_Recording ? 1 : 0);
        serial.print(" P");
        serial.print(m_Muted ? 0 : 1);
        serial.print(" S");
        serial.print(m_NoteStack.Size());
        serial.print(" #");
        serial.println(m_NumItems);
    }

    uint8_t m_MidiChannel;
    bool m_MidiLearn;
    bool m_Recording;
    bool m_Muted;
    //TODO int m_ClockDivider;

    MidiNoteStack m_NoteStack;

    static const int ItemCapacity = 128;
    MidiLooperItem m_Items[ItemCapacity];
    int m_NumItems;
};
