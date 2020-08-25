#pragma once
#include <Arduino.h>
#include "MidiOut.h"
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

        // TODO all playing notes should be note off???
        // TODO all 'cleared' note off events should be sent right now!
        // TODO only those after current step??
        m_NumItems = 0;
    }

    void onToggleMuted()
    {
        m_Muted = !m_Muted;
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

    void onTick(const MidiLooperTicker &ticker, MidiOut &midiOut)
    {
        if (ticker.clockIsRising())
        {
            //TODO if muted => all note off?
            //TODO if channel changed (midi learn) => all notes off on original channel?
            int step = ticker.Counter(1); //TODO divider

            // iterate all items, if step matches current step, play it
            // muted => no note on, but still do all note off (?)
            int idx = 0;
            while (idx < m_NumItems)
            {
                if (step == m_Items[idx].m_Step)
                {
                    if (m_Items[idx].m_Velocity)
                    {
                        if (!m_Muted)
                        {
                            midiOut.NoteOn(m_MidiChannel, m_Items[idx].m_MidiNote, m_Items[idx].m_Velocity);
                        }
                    }
                    else
                    {
                        // use some dummy velocity but not zero!
                        midiOut.NoteOff(m_MidiChannel, m_Items[idx].m_MidiNote, 0x01);
                    }
                }
                ++idx;
            }
        }
    }

    void onNoteOn(const MidiLooperTicker &ticker, uint8_t midiChannel, uint8_t midiNote, uint8_t velocity)
    {
        //TODO if midi learn changes channels, need to note off all open notes!
        if (m_MidiLearn)
        {
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

    uint8_t m_MidiChannel;
    bool m_MidiLearn;
    bool m_Recording;
    bool m_Muted;
    //TODO int m_ClockDivider;

    static const int ItemCapacity = 128;
    MidiLooperItem m_Items[ItemCapacity];
    int m_NumItems;
};
