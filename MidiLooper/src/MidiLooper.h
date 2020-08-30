#include <Arduino.h>

#include "MidiHandler.h"
#include "MidiOut.h"
#include "MidiLooperTicker.h"
#include "MidiLooperTrack.h"
#include "MidiMetronome.h"

class MidiLooper : public MidiHandler
{
public:
    MidiLooper() : m_MidiOut(), m_Ticker(), m_Track() {}

    void begin(HardwareSerial *midiSerial)
    {
        m_MidiOut.begin(midiSerial);
        for (int idx = 0; idx < NumTracks; ++idx)
        {
            m_Track[idx].m_MidiLearn = false;
            m_Track[idx].m_MidiLearn = idx;
        }
        m_Track[4].m_MidiChannel = 0x09;
        m_Track[5].m_MidiChannel = 0x0A;
        //TODO default channel for m_Metronome
    }

    void onTick(int clock, int reset)
    {
        m_Ticker.onTick(clock, reset);
        m_Metronome.OnTick(m_Ticker, m_MidiOut);
        for (int idx = 0; idx < NumTracks; ++idx)
        {
            m_Track[idx].onTick(m_Ticker, m_MidiOut);
        }
    }

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
    {
        m_MidiOut.NoteOn(Channel, MidiNote, Velocity);
        for (int idx = 0; idx < NumTracks; ++idx)
        {
            m_Track[idx].onNoteOn(m_Ticker, m_MidiOut, Channel, MidiNote, Velocity);
        }
        m_Metronome.OnNoteOn(Channel, MidiNote, Velocity);
    }
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
    {
        m_MidiOut.NoteOff(Channel, MidiNote, Velocity);
        for (int idx = 0; idx < NumTracks; ++idx)
        {
            m_Track[idx].onNoteOff(m_Ticker, Channel, MidiNote, Velocity);
        }
    }
    void MidiStart()
    {
        m_Metronome.Start();
        for (int idx = 0; idx < NumTracks; ++idx)
        {
            m_Track[idx].StartRecording();
        }
    }
    void MidiStop()
    {
        m_Metronome.Stop();
        for (int idx = 0; idx < NumTracks; ++idx)
        {
            m_Track[idx].StopRecording();
        }
    }

    void allNotesOff()
    {
        for (int idx = 0; idx < NumTracks; ++idx)
        {
            m_MidiOut.allNotesOff(m_Track[idx].m_MidiChannel);
        }
    }

    void printState(HardwareSerial &serial)
    {
        for (int idx = 0; idx < NumTracks; ++idx)
        {
            serial.print("Track ");
            serial.print(idx);
            serial.print(": ");
            m_Track[idx].printState(serial);
        }
    }

    MidiOut m_MidiOut;
    MidiLooperTicker m_Ticker;
    static const int NumTracks = 6;
    MidiLooperTrack m_Track[NumTracks];
    MidiMetronome m_Metronome;
};