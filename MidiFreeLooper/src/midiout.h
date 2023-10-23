#pragma once
#include <Arduino.h>

class MidiNoteMask;
class MidiVoiceMessage;

//void NoteOn(HardwareSerial& serial, uint8_t channel, uint8_t midiNote, uint8_t velocity);
//void NoteOff(HardwareSerial& serial, uint8_t channel, uint8_t midiNote, uint8_t velocity);

void NoteOn(HardwareSerial& serial, MidiNoteMask& mask, uint8_t channel, uint8_t midiNote, uint8_t velocity);
void NoteOff(HardwareSerial& serial, MidiNoteMask& mask, uint8_t channel, uint8_t midiNote, uint8_t velocity);

class MidiOut
{
public:
    MidiOut();
    void Begin(HardwareSerial* serial);
    void VoiceMessage(const MidiVoiceMessage& message);
    void NoteOff(uint8_t note, uint8_t channel, uint8_t velocity);
private:
    HardwareSerial* m_Serial;
};
