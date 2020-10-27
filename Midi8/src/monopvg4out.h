#pragma once

#include <Arduino.h>
#include "MidiNoteStack.h"

class Midi8UI;

class MonoPVG4Out
{
public:
    MonoPVG4Out();

    void Begin(int idx);

    void NoteOn(uint8_t channel, uint8_t midiNote, uint8_t velocity);
    void NoteOff(uint8_t channel, uint8_t midiNote, uint8_t velocity);
    void Learn(bool learn);
    bool IsLearning() const;

    void updateUI(Midi8UI* ui);

private:
    int m_Idx;

    bool m_Learn;

    uint8_t m_Channel;
    uint8_t m_BaseNote;
    MidiNoteVelocityStack m_Stack;//mono => stack
};
