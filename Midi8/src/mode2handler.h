#pragma once

#include <Arduino.h>
#include "MidiHandler.h"
#include "monopg2out.h"

class Midi8UI;

class Mode2Handler : public MidiHandler
{
public:
    Mode2Handler();

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);//override
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) ;//override

    void updateUI(Midi8UI* ui);

private:
    static const int Size = 4;

    MonoPG2Out m_Out[Size];

    int m_LearnIdx;
};
