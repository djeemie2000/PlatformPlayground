#include "MidiNoteState.h"

MidiNoteState::MidiNoteState(){}

    void MidiNoteState::Tick()
    {
        ++m_Tick;
    }
    void MidiNoteState::NoteOn(uint8_t MidiNote, uint8_t Velocity)
    {
        NoteOn(MidiNote, Velocity, 0);
    }
    void MidiNoteState::NoteOn(uint8_t MidiNote, uint8_t Velocity, int Transpose)
    {
        // no check on note in range
        if(!m_State[MidiNote].Pressed)
        {
            ++m_NumPressed;
            m_State[MidiNote].Tick = m_Tick;
            m_State[MidiNote].Pressed = true;
        }
        m_State[MidiNote].Velocity = Velocity;
        m_State[MidiNote].Transpose = Transpose;
    }
    void MidiNoteState::NoteOff(uint8_t MidiNote, uint8_t Velocity)
    {
        // no check on note in range
        if(m_State[MidiNote].Pressed)
        {
            --m_NumPressed;
            m_State[MidiNote].Tick = m_Tick-m_State[MidiNote].Tick;
            m_State[MidiNote].Pressed = false;
        }
        m_State[MidiNote].Velocity = Velocity;
    }

    int MidiNoteState::NumPressed() const
    {
        return m_NumPressed;
    }
    bool MidiNoteState::NotePressed(int MidiNote) const
    {
        // no check on note in range
        return m_State[MidiNote].Pressed;
    }
    uint8_t MidiNoteState::Velocity(int MidiNote) const
    {
        // no check on note in range
        return m_State[MidiNote].Velocity;
    }
    uint32_t MidiNoteState::PressDuration(int MidiNote) const
    {
        return !m_State[MidiNote].Pressed ? m_State[MidiNote].Tick :  m_Tick-m_State[MidiNote].Tick;

    }
    int MidiNoteState::Transpose(int MidiNote) const
    {
        // no check on note in range
        return m_State[MidiNote].Transpose;
    }

