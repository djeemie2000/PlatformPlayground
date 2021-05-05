#include "StackMidiOut.h"

StackMidiOut::StackMidiOut() : m_MidiOut(), m_NoteStack() {}

void StackMidiOut::begin(HardwareSerial *serial)
{
    m_MidiOut.begin(serial);
}

void StackMidiOut::NoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    m_MidiOut.NoteOn(channel, note, velocity);
    m_NoteStack.NoteOn(note);
}

void StackMidiOut::NoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
    m_MidiOut.NoteOff(channel, note, velocity);
    m_NoteStack.NoteOff(note);
}

void StackMidiOut::ContinuousController(uint8_t channel, uint8_t controller, uint8_t value)
{
    m_MidiOut.ContinuousController(channel, controller, value);
}

void StackMidiOut::allNotesOff(uint8_t channel)
{
    //assumes all same channel!!!
    while (m_NoteStack.Size())
    {
        // fixed non zero velocity
        m_MidiOut.NoteOff(channel, m_NoteStack.Pop(), 0x01);
    }
}
