#pragma once

#include <Arduino.h>
#include "MidiHandler.h"

class Midi10UI;

class Poly4Handler : public MidiHandler
{
public:
    Poly4Handler();

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);  //override
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity); //override

    void updateUI(Midi10UI *ui);
    bool IsLearning() const;
    void Learn(bool learn);

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

private:
    static const int Size = 2;

    uint8_t m_Channel;
    uint8_t m_BaseNote;
    uint8_t m_MidiNote[Size];
    uint8_t m_Velocity[Size];

    int m_LearnIdx;
};
