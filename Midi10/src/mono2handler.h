#pragma once

#include <Arduino.h>
#include "MidiHandler.h"
#include "monopg2out.h"

class Midi10UI;

class Mono2Handler : public MidiHandler
{
public:
    Mono2Handler();

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);  //override
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity); //override

    void updateUI(Midi10UI *ui);
    bool IsLearning() const;
    void Learn(bool learn);

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

private:
    static const int Size = 4;

    MonoPG2Out m_Out[Size];

    int m_LearnIdx;
};
