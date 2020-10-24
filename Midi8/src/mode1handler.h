#pragma once

#include <Arduino.h>
#include "MidiHandler.h"

class Midi8UI;

class Mode1Handler : public MidiHandler
{
public:
    Mode1Handler();

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);//override
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) ;//override

    void updateUI(Midi8UI* ui);

private:
    static const int Size = 8;

    uint8_t m_Channel[Size];
    uint8_t m_MidiNote[Size];
    int m_Gate[Size];

    int m_LearnIdx;
};
