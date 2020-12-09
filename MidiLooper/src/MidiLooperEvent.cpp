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
