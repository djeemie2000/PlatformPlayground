#pragma once
#include <Arduino.h>

struct MidiVoiceMessage
{
    uint8_t StatusByte;
    uint8_t Param1;
    uint8_t Param2;
};

uint8_t Command(MidiVoiceMessage& message);
uint8_t Channel(MidiVoiceMessage& message);
uint8_t MidiNote(MidiVoiceMessage& message);
uint8_t Velocity(MidiVoiceMessage& message);
bool IsNoteOn(MidiVoiceMessage& message);
bool IsNoteOff(MidiVoiceMessage& message);
bool IsController(MidiVoiceMessage& message);
