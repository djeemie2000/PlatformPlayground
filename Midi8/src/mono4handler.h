#pragma once

#include <Arduino.h>
#include "MidiHandler.h"
#include "monopvg4out.h"

class Midi8UI;

class Mono4Handler : public MidiHandler
{
public:
    Mono4Handler();

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);//override
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) ;//override

    void updateUI(Midi8UI* ui);
    bool IsLearning() const;

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

private:
    static const int Size = 2;

    MonoPVG4Out m_Out[Size];

    int m_LearnIdx;
};
