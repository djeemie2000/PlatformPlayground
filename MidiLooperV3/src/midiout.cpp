#include "midiout.h"
#include "midinotemask.h"

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
