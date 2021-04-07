#pragma once

#include <Arduino.h>
#include "MidiHandler.h"

class Midi8UI;

class Single2Handler : public MidiHandler
{
public:
    Single2Handler();

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);//override
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) ;//override

    void updateUI(Midi8UI* ui);
    bool IsLearning() const;

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

private:
    static const int Size = 4;

    uint8_t m_Channel[Size];
    uint8_t m_MidiNote[Size];
    int m_Gate[Size];
    uint8_t m_Velocity[Size];

    int m_LearnIdx;
};
