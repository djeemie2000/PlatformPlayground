#pragma once

#include <Arduino.h>
#include "MidiNoteStack.h"

class Midi8UI;

class MonoPVG4Out
{
public:
    MonoPVG4Out();

    void Begin(int idx, Midi8UI* ui);

    void NoteOn(uint8_t channel, uint8_t midiNote, uint8_t velocity);
    void NoteOff(uint8_t channel, uint8_t midiNote, uint8_t velocity);
    void Learn(bool learn);
    bool IsLearning() const;

    void updateUI(Midi8UI* ui);

private:

    int m_Idx;
    Midi8UI* m_Ui;

    bool m_Learn;

    uint8_t m_Channel;
    MidiNoteVelocityStack m_Stack;//mono => stack
};
