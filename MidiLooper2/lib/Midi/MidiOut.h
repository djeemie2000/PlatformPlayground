#pragma once

#include <Arduino.h>
#include "MidiNoteOnStack.h"

class MidiOut
{
public:
    MidiOut();

    void begin(HardwareSerial *serial);

    void NoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    void NoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
    void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value);
    void MidiClock();
    void MidiStart();
    void MidiContinue();
    void MidiStop();
    
    void allNotesOff(uint8_t channel);

private:
    HardwareSerial *m_Serial;
    MidiNoteOnStack m_NoteOnStack[16];//per midi channel
};
