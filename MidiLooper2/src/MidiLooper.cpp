#include "MidiLooper.h"
#include "MidiLooperStorage.h"

MidiLooper::MidiLooper() : m_MidiOut(), m_Ticker(), m_Track(), m_IsRunning(true) {}

void MidiLooper::begin(HardwareSerial *midiSerial)
{
    m_MidiOut.begin(midiSerial);
}

void MidiLooper::onTick(int clock)
{
    if(!m_IsRunning)
    {
        return;
    }

    m_Ticker.onTick(clock);
    
    // midi clock sync (not midi standard compliant !!!): 
    //      the resulting midi clock is 4PPQ instead of 24PPQ
    // midi clock upon each rising clock
    // when counter == 0 preceed midi clock by a midi continue which indicates a reset pulse
    if(m_Ticker.clockIsRising())
    {
        if(m_Ticker.Counter()==0)
        {
            m_MidiOut.MidiContinue();
        }
        m_MidiOut.MidiClock();
    }

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
    m_MidiOut.MidiStart();
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

    m_MidiOut.MidiStop();
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

void MidiLooper::Save(MidiLooperStorage& storage, uint8_t slot)
{
    // iterate tracks:
    storage.Open(slot);
    m_Metronome.Save(storage, 0xFF);
    for(int track = 0; track<MidiLooper::NumTracks; ++track)
    {
        m_Track[track].Save(storage, track);
    }
    storage.Close();
}

void MidiLooper::Load(MidiLooperStorage& storage, uint8_t slot)
{
    // iterate tracks:
    storage.Open(slot);
    m_Metronome.Load(storage, 0xFF);
    for(int track = 0; track<MidiLooper::NumTracks; ++track)
    {
        m_Track[track].Load(storage, track);
    }
    storage.Close();
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
