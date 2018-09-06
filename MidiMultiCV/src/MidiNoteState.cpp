#include "MidiNoteState.h"

MidiNoteState::MidiNoteState(){}

    void MidiNoteState::Tick()
    {
        ++m_Tick;
    }
    void MidiNoteState::NoteOn(uint8_t MidiNote, uint8_t Velocity)
    {
        // no check on note in range
        if(!m_State[MidiNote].Pressed)
        {
            ++m_NumPressed;
            m_State[MidiNote].TickOn = m_Tick;
            m_State[MidiNote].Pressed = true;
        }
        m_State[MidiNote].Velocity = Velocity;
    }
    void MidiNoteState::NoteOff(uint8_t MidiNote, uint8_t Velocity)
    {
        // no check on note in range
        if(m_State[MidiNote].Pressed)
        {
            --m_NumPressed;
            m_State[MidiNote].TickOff = m_Tick;
            m_State[MidiNote].Pressed = false;
        }
        m_State[MidiNote].Velocity = Velocity;
    }

    int MidiNoteState::NumPressed() const
    {
        return m_NumPressed;
    }
    bool MidiNoteState::NotePressed(int MidiNote)
    {
        // no check on note in range
        return m_State[MidiNote].Pressed;
    }
    uint8_t MidiNoteState::Velocity(int MidiNote)
    {
        // no check on note in range
        return m_State[MidiNote].Velocity;
    }
    uint32_t MidiNoteState::PressDuration(int MidiNote) const
    {
        return !m_State[MidiNote].Pressed ? m_State[MidiNote].TickOff- m_State[MidiNote].TickOn :  m_Tick-m_State[MidiNote].TickOn;
    }
