#pragma once
#include <Arduino.h>

class MidiNoteMask;

//void NoteOn(HardwareSerial& serial, uint8_t channel, uint8_t midiNote, uint8_t velocity);
//void NoteOff(HardwareSerial& serial, uint8_t channel, uint8_t midiNote, uint8_t velocity);

void NoteOn(HardwareSerial& serial, MidiNoteMask& mask, uint8_t channel, uint8_t midiNote, uint8_t velocity);
void NoteOff(HardwareSerial& serial, MidiNoteMask& mask, uint8_t channel, uint8_t midiNote, uint8_t velocity);
