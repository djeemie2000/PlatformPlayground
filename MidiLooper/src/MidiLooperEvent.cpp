#include "MidiLooperTrack.h"

bool MidiLooperEvent::IsNoteOn() const { return m_Velocity != NoteOffVelocity; }

bool MidiLooperEvent::IsNoteOff() const { return m_Velocity == NoteOffVelocity; }

void MidiLooperEvent::SetNoteOn(uint8_t step, uint8_t midiNote, uint8_t velocity, uint8_t layer)
{
    m_Step = step;
    m_MidiNote = midiNote;
    m_Velocity = velocity;
    m_Layer = layer;
}

void MidiLooperEvent::SetNoteOff(uint8_t step, uint8_t midiNote, uint8_t /*velocity*/, uint8_t layer)
{
    m_Step = step;
    m_MidiNote = midiNote;
    m_Velocity = NoteOffVelocity;
    m_Layer = layer;
}

 void MidiLooperNoteEvent::SetNoteOn(uint8_t step, uint8_t midiNote, uint8_t velocity, uint8_t layer)
 {
    m_MidiNote = midiNote;
    m_VelocityOn = velocity;
    m_Layer = layer;
    m_StepOn = step;
    m_VelocityOff = 0xFF;
 }

bool MidiLooperNoteEvent::SetNoteOff(uint8_t step, uint8_t midiNote, uint8_t velocity)
{
    if(midiNote == m_MidiNote)
    {
        m_StepOff = step;
        m_VelocityOff = velocity;
        return true;
    }
    return false;
}

bool MidiLooperNoteEvent::HasNoteOff() const
{
    return m_VelocityOff != 0xFF;
}

bool MidiLooperNoteEvent::IsValid() const
{
    return m_VelocityOff != 0xFF && m_StepOn!=m_StepOff;
}

