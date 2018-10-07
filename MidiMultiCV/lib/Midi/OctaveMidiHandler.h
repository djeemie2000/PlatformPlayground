#pragma once

#include <mbed.h>
#include "MidiHandler.h"
#include "VoltageOut.h"
#include "MidiNoteState.h"

uint8_t Transpose(uint8_t MidiNote, int transp);

class OctaveMidiHandler : public MidiHandler
{
public:
    OctaveMidiHandler(MidiHandler& handler) 
    : m_Handler(handler)
    {}

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
    {
        m_NoteState.NoteOn(MidiNote, Velocity, 12*m_OctaveOffset);
        m_Handler.NoteOn(Channel, Transpose(MidiNote, 12*m_OctaveOffset), Velocity);
    }
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
    {
        // problem : what if octave is changed between note on and note off?
        // solution: remember in note state
        uint8_t note = Transpose(MidiNote, m_NoteState.Transpose(MidiNote));
        m_NoteState.NoteOff(MidiNote, Velocity);
        m_Handler.NoteOff(Channel, note, Velocity);
    }
    void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
    {
        if(m_Controller==128)
        {
            m_Controller = Controller;
        }
        if(m_Controller == Controller)
        {
            //assume all CC is voltage offset change
            m_OctaveOffset = (Value>>3);//[0,15]
            m_OctaveOffset -= 8;
        } else
        {
            m_Handler.ContinuousController(Channel, Controller, Value);
        }
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
    uint8_t m_Controller{128};
    int m_OctaveOffset{-4};
};
