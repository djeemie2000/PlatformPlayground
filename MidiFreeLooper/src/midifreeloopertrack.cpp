#include "midifreeloopertrack.h"
#include "midivoicemessage.h"
#include "midiout.h"

MidiFreeLooperTrack::MidiFreeLooperTrack()
 : m_PlayMute(false)
 , m_RecordArm(0)
 , m_ResetFlag(true)
 , m_Length24PPQ(-1)
 , m_Counter24PPQ(0)
 , m_Events(MidiVoiceMessage())//TODO !!!!!
{}

void MidiFreeLooperTrack::AllNotesOff(MidiOut& midiOut)
{
    while(0<m_NoteStack.Size())
    {
        uint16_t value = m_NoteStack.Pop();
        //velocity 0x01 or 0x00??
        midiOut.NoteOff(m_NoteStack.ToNote(value), m_NoteStack.ToChannel(value), 0x01);
    }
}

void MidiFreeLooperTrack::MessageOut(MidiOut& midiOut, const MidiVoiceMessage& message)
{
    uint8_t note = MidiNote(message);
    uint8_t channel = Channel(message);
    bool contains = m_NoteStack.Contains(note, channel);
    if(IsNoteOn(message) && !contains)
    {
        m_NoteStack.NoteOn(note, channel);
        midiOut.VoiceMessage(message);
    }
    else if(IsNoteOff(message) && contains)
    {
        m_NoteStack.NoteOff(note, channel);
        midiOut.VoiceMessage(message);
    }
}

void MidiFreeLooperTrack::OnReset(MidiOut& midiOut)
{
    if(m_RecordArm == 2)
    {
        m_Length24PPQ = m_Counter24PPQ + 1;
    }
    AllNotesOff(midiOut);
    m_ResetFlag = true;
}
    
void MidiFreeLooperTrack::OnTogglePlayMute(MidiOut& midiOut)
{
    m_PlayMute = !m_PlayMute;
    if(!m_PlayMute)
    {
        AllNotesOff(midiOut);
    }
}

void MidiFreeLooperTrack::OnToggleRecording()
{
    // arm 1 record 2 => off 0
    if(m_RecordArm == 2)
    {
        m_RecordArm = 0;
        //TODO perhaps add note off at end of loop for all current note on???
    }
    else if(m_RecordArm == 1)
    {
        m_RecordArm = 2;
    }
    else
    {
        // 0==length => arm 
        // 0<length => record
        m_RecordArm = 0<m_Length24PPQ ? 2 : 1;
    }
}

void MidiFreeLooperTrack::OnMidiClock(MidiOut& midiOut)
{
    // update counter
    if(m_ResetFlag)
    {
        m_Counter24PPQ = 0;
        m_ResetFlag = true;
        // TODO reset head on event list
        m_Events.ResetHead();
    }
    else
    {
        ++m_Counter24PPQ;
        if(0<m_Length24PPQ && m_Length24PPQ<=m_Counter24PPQ)
        {
            m_Counter24PPQ = 0;
        }
    }

    if(0==m_Counter24PPQ)
    {        
        // in case note on was recorded in loop but note off is not recorded
        AllNotesOff(midiOut);
    }

    // play all ~current counter
    // if muted, still need to iterate all ~current counter!
    while(m_Events.MoveHeadToNextIfNextKeyEquals(m_Counter24PPQ))
    {
        if(m_PlayMute)
        {
            // TODO note stack!
            MessageOut(midiOut, m_Events.HeadValue());
        }
    }
}
    
void MidiFreeLooperTrack::OnMidiNoteEvent(MidiVoiceMessage& message)
{
    // arm => start recording upon first note
    if(1 == m_RecordArm)
    {
        m_RecordArm = 2;
    }
    // if recording, add event 
    // but only if note on/note off
    if(2 == m_RecordArm)
    {
        //add to events
        if(IsNoteOn(message) || IsNoteOff(message))
        {
            m_Events.Add(m_Counter24PPQ, message);
        }
    }
}
