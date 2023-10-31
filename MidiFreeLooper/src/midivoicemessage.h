#pragma once
#include <Arduino.h>

struct MidiVoiceMessage
{
    uint8_t StatusByte;
    uint8_t Param1;
    uint8_t Param2;
};

uint8_t Command(const MidiVoiceMessage& message);
uint8_t Channel(const MidiVoiceMessage& message);
uint8_t MidiNote(const MidiVoiceMessage& message);
uint8_t Velocity(const MidiVoiceMessage& message);
bool IsNoteOn(const MidiVoiceMessage& message);
bool IsNoteOff(const MidiVoiceMessage& message);
bool IsController(const MidiVoiceMessage& message);

void PrintVoiceMessage(const MidiVoiceMessage &message, HardwareSerial& serialDebug);
