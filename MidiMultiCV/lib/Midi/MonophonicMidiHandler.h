#pragma once

#include <mbed.h>
#include "MidiHandler.h"
#include "VoltageOut.h"
#include "MidiNoteState.h"
#include "MidiNoteStack.h"


class MonophonicMidiHandler : public MidiHandler
{
public:
    MonophonicMidiHandler(MidiHandler& handler) 
    : m_Handler(handler)
    {}

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override;

    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override;
    
    void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
    {
        m_Handler.ContinuousController(Channel, Controller, Value);
    }
    void AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure)
    {
        m_Handler.AfterTouch(Channel, MidiNote, Pressure);
    }
    void ProgramChange(uint8_t Channel, uint8_t Program) 
    {
        m_Handler.ProgramChange(Channel, Program);
    }
    void ChannelPressure(uint8_t Channel, uint8_t Pressure) 
    {
        m_Handler.ChannelPressure(Channel, Pressure);
    }
    void PitchWheel(uint8_t Channel, int Bend) 
    {
        m_Handler.PitchWheel(Channel, Bend);
    }
    //TODO pass all other functions to handler!

private:
    MidiHandler& m_Handler;
    MidiNoteState m_NoteState;
    MidiNoteStack m_Stack;
};
