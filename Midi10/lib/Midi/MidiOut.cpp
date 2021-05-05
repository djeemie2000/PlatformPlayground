#include "MidiOut.h"

MidiOut::MidiOut() : m_Serial(nullptr) {}

void MidiOut::begin(HardwareSerial *serial)
{
    m_Serial = serial;
    m_Serial->begin(31250); //midi baudrate 31250
}

void MidiOut::NoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t statusByte = 0x90 | channel;
    uint8_t msg[3] = {statusByte, note, velocity};
    m_Serial->write(msg, 3);
}

void MidiOut::NoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t statusByte = 0x80 | channel;
    uint8_t msg[3] = {statusByte, note, velocity};
    m_Serial->write(msg, 3);
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
