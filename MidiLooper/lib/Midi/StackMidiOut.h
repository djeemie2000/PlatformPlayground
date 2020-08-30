#pragma once

#include <Arduino.h>
#include "MidiOut.h"
#include "MidiNoteStack.h"

class StackMidiOut
{
public:
    StackMidiOut();

    void begin(HardwareSerial *serial);

    void NoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    void NoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
    void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value);

    void allNotesOff(uint8_t channel);

private:
    MidiOut m_MidiOut;
    MidiNoteStack m_NoteStack;
};
