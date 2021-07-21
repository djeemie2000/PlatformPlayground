#include "MidiOut.h"

MidiOut::MidiOut() : m_Serial(nullptr) {}

void MidiOut::begin(HardwareSerial *serial)
{
    m_Serial = serial;
    m_Serial->begin(31250); //midi baudrate 31250
}

void MidiOut::NoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if(!m_NoteOnStack[channel].GetNoteOn(note))
    {
        uint8_t statusByte = 0x90 | channel;
        uint8_t msg[3] = {statusByte, note, velocity};
        m_Serial->write(msg, 3);

        m_NoteOnStack[channel].SetNoteOn(note);
    }
}

void MidiOut::NoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
    if(m_NoteOnStack[channel].GetNoteOn(note))
    {
        uint8_t statusByte = 0x80 | channel;
        uint8_t msg[3] = {statusByte, note, velocity};
        m_Serial->write(msg, 3);

        m_NoteOnStack[channel].SetNoteOff(note);
    }
}

void MidiOut::ContinuousController(uint8_t channel, uint8_t controller, uint8_t value)
{
    uint8_t statusByte = 0xB0 | channel;
    uint8_t msg[3] = {statusByte, controller, value};
    m_Serial->write(msg, 3);
}

void MidiOut::allNotesOff(uint8_t channel)
{
    // midi CC where cc=123 and value = 0
    ContinuousController(channel, 0x7B, 0x00);
}

void MidiOut::MidiClock()
{
    m_Serial->write(0xF8);
}

void MidiOut::MidiStart()
{
    m_Serial->write(0xFA);
}

void MidiOut::MidiContinue()
{
    m_Serial->write(0xFB);
}

void MidiOut::MidiStop()
{
    m_Serial->write(0xFC);
}