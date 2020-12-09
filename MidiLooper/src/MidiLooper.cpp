#include "MidiLooper.h"

MidiLooper::MidiLooper() : m_MidiOut(), m_Ticker(), m_Track() {}

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
    m_Ticker.onTick(clock, reset);
    m_Metronome.OnTick(m_Ticker, m_MidiOut);
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_Track[idx].onTick(m_Ticker, m_MidiOut);
    }
}

void MidiLooper::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    // no note on if some track is recording + clock low
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
    m_Metronome.Start();
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_Track[idx].StartRecording();
    }
}

void MidiLooper::MidiStop()
{
    m_Metronome.Stop();
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_Track[idx].StopRecording();
    }
}

void MidiLooper::allNotesOff()
{
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        m_MidiOut.allNotesOff(m_Track[idx].m_MidiChannel);
    }
}

void MidiLooper::printState(HardwareSerial &serial)
{
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        serial.print("Track ");
        serial.print(idx);
        serial.print(": ");
        m_Track[idx].printState(serial);
    }
}
