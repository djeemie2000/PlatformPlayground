#include "MonophonicMidiHandler.h"

void MonophonicMidiHandler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    m_NoteState.NoteOn(MidiNote, Velocity);
    m_Stack.NoteOn(MidiNote);

    uint8_t Note = m_Stack.CurrentNote();
    m_Handler.NoteOn(Channel, Note, m_NoteState.Velocity(Note));
}

void MonophonicMidiHandler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    // problem : what if octave is changed between note on and note off?
    // solution: remember in note state
    m_NoteState.NoteOff(MidiNote, Velocity);
    m_Stack.NoteOff(MidiNote);
    
    if(0<m_Stack.Size())
    {
        // in this case no noteoff to prevent clicks in gate of monophonic handler?
        uint8_t Note = m_Stack.CurrentNote();
        m_Handler.NoteOn(Channel, Note, m_NoteState.Velocity(Note));
    }
    else 
    {
        m_Handler.NoteOff(Channel, MidiNote, Velocity);
    }
}
