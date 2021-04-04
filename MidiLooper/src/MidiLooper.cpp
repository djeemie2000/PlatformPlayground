#include "MidiLooper.h"

MidiLooper::MidiLooper() : m_MidiOut(), m_Ticker(), m_Track(), m_IsRunning(true) {}

void MidiLooper::begin(HardwareSerial *midiSerial)
{
    m_MidiOut.begin(midiSerial);
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_Track[idx].m_MidiLearn = false;
        m_Track[idx].m_MidiChannel = idx;
    }
    m_Track[0].m_MidiChannel = 0x09;
    m_Track[1].m_MidiChannel = 0x0A;
    //TODO default channel for m_Metronome
}

void MidiLooper::onTick(int clock, int reset)
{
    if(!m_IsRunning)
    {
        return;
    }

    m_Ticker.onTick(clock, reset);
    m_Metronome.OnTick(m_Ticker, m_MidiOut);
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_Track[idx].onTick(m_Ticker, m_MidiOut);
    }
}

void MidiLooper::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    m_MidiOut.NoteOn(Channel, MidiNote, Velocity);
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_Track[idx].onNoteOn(m_Ticker, m_MidiOut, Channel, MidiNote, Velocity);
    }
    m_Metronome.OnNoteOn(Channel, MidiNote, Velocity);
}

void MidiLooper::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    m_MidiOut.NoteOff(Channel, MidiNote, Velocity);
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_Track[idx].onNoteOff(m_Ticker, Channel, MidiNote, Velocity);
    }
}

void MidiLooper::MidiStart()
{
    m_IsRunning = true;
    m_Ticker.reset();
}

void MidiLooper::MidiStop()
{
    // stop running i.e. ignore clock
    m_IsRunning = false;

    // all currently playing notes off
    m_Metronome.AllNotesOff(m_MidiOut);
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_Track[idx].AllNotesOff(m_MidiOut);
    }
}

void MidiLooper::allNotesOff()
{
    m_Metronome.AllNotesOff(m_MidiOut);//??
    //m_MidiOut.allNotesOff(m_Metronome.m_MidiChannel);//TODO??
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_MidiOut.allNotesOff(m_Track[idx].m_MidiChannel);
    }
}

void MidiLooper::ToggleRecording(int idxTrack)
{
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        if(idx == idxTrack)
        {
            m_Track[idx].ToggleRecording();
        }
        else
        {
            m_Track[idx].StopRecording();
        }
    }
}

void MidiLooper::ToggleMidiLearn(int idxTrack)
{
     for (int idx = 0; idx < NumTracks; ++idx)
    {
        if(idx == idxTrack)
        {
            m_Track[idx].ToggleMidiLearn();
        }
        else
        {
            m_Track[idx].StopMidiLearn();
        }
    }
}



void MidiLooper::printState(HardwareSerial &serial)
{
    serial.print("Running: ");
    serial.println(m_IsRunning?1:0);
    serial.print("Metro  : ");
    m_Metronome.printState(serial);
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        serial.print("Track ");
        serial.print(idx);
        serial.print(": ");
        m_Track[idx].printState(serial);
    }
}
