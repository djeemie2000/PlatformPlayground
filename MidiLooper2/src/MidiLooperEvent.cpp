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

//  void MidiLooperNoteEvent::SetNoteOn(uint8_t step, uint8_t midiNote, uint8_t velocity, uint8_t layer)
//  {
//     m_MidiNote = midiNote;
//     m_VelocityOn = velocity;
//     m_Layer = layer;
//     m_StepOn = step;
//     m_VelocityOff = 0xFF;
//  }

// bool MidiLooperNoteEvent::SetNoteOff(uint8_t step, uint8_t midiNote, uint8_t velocity)
// {
//     if(midiNote == m_MidiNote)
//     {
//         m_StepOff = step;
//         m_VelocityOff = velocity;
//         return true;
//     }
//     return false;
// }

// bool MidiLooperNoteEvent::HasNoteOff() const
// {
//     return m_VelocityOff != 0xFF;
// }

// bool MidiLooperNoteEvent::IsValid() const
// {
//     return m_VelocityOff != 0xFF && m_StepOn!=m_StepOff;
// }

// MidiLooperStepStack::MidiLooperStepStack() : m_SizeOn(0), m_SizeOff(0)
// {}

// bool MidiLooperStepStack::OnNoteOn(uint8_t midiNote, uint8_t velocity, uint8_t layer)
// {
//     if(m_SizeOn<Capacity)
//     {
//         m_MidiNoteOn[m_SizeOn] = midiNote;
//         m_Velocity[m_SizeOn] = velocity;
//         m_LayerOn[m_SizeOn] = layer;
//         ++m_SizeOn;
//         return true;
//     }
//     return false;
// }

// bool MidiLooperStepStack::OnNoteOff(uint8_t midiNote, uint8_t /*velocity*/, uint8_t layer)
// {
//     // check if no note on for same midi note TODO
//     if(m_SizeOff<Capacity)
//     {
//         m_MidiNoteOff[m_SizeOff] = midiNote;
//         m_LayerOff[m_SizeOff] = layer;
//         ++m_SizeOff;
//         return true;
//     }
//     return false;
// }

// void MidiLooperStepStack::OnUndo(uint8_t layer)
// {
//     while(0<m_SizeOn && m_LayerOn[m_SizeOn-1] == layer)
//     {
//         --m_SizeOn;
//     }
//     while(0<m_SizeOff && m_LayerOff[m_SizeOff-1] == layer)
//     {
//         --m_SizeOff;
//     }
// }
