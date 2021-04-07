#pragma once

#include <Arduino.h>
#include "MidiNoteStack.h"

class Midi8UI;

class MonoPG2Out
{
public:
    MonoPG2Out();

    void Begin(int idx);

    void NoteOn(uint8_t channel, uint8_t midiNote);
    void NoteOff(uint8_t channel, uint8_t midiNote);
    void Learn(bool learn);
    bool IsLearning() const;

    void updateUI(Midi8UI* ui);

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);
    
private:
    int m_Idx;

    bool m_Learn;

    uint8_t m_Channel;
    uint8_t m_BaseNote;
    MidiNoteStack m_Stack;//mono => stack
};
