#include "midiout.h"
#include "midinotemask.h"
#include "midivoicemessage.h"

void NoteOn(HardwareSerial& serial, uint8_t channel, uint8_t midiNote, uint8_t velocity)
{
    serial.write(0x90 | (channel &0x0F));
    serial.write(midiNote);
    serial.write(velocity);
}

void NoteOff(HardwareSerial& serial, uint8_t channel, uint8_t midiNote, uint8_t velocity)
{
    serial.write(0x80 | (channel &0x0F));
    serial.write(midiNote);
    serial.write(velocity);
}

void NoteOn(HardwareSerial& serial, MidiNoteMask& mask, uint8_t channel, uint8_t midiNote, uint8_t velocity)
{
    if(!mask.IsNoteOn(midiNote))
    {
        NoteOn(serial, channel, midiNote, velocity);
        mask.NoteOn(midiNote);
    };
}

void NoteOff(HardwareSerial& serial, MidiNoteMask& mask, uint8_t channel, uint8_t midiNote, uint8_t velocity)
{
    if(mask.IsNoteOn(midiNote))
    {
        NoteOff(serial, channel, midiNote, velocity);
        mask.NoteOff(midiNote);
    }
}

MidiOut::MidiOut() 
: m_Serial(0)
{}

void MidiOut::Begin(HardwareSerial* serial)
{
    m_Serial = serial;
}

void MidiOut::VoiceMessage(const MidiVoiceMessage& message)
{
    m_Serial->write(message.StatusByte);
    m_Serial->write(message.Param1);
    m_Serial->write(message.Param2);
}

void MidiOut::NoteOff(uint8_t note, uint8_t channel, uint8_t velocity)
{
    m_Serial->write(0x80 | channel);
    m_Serial->write(note);
    m_Serial->write(velocity);
}
