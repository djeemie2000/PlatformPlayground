#pragma once

#include <Arduino.h>
#include "MidiHandler.h"
#include "monopvg4out.h"

class Midi10UI;
class ClockSyncOut;

class Mono4Handler : public MidiHandler
{
public:
    Mono4Handler();
    void begin(ClockSyncOut *clockSyncOut);

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);  //override
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity); //override
    void MidiClock();                                                  //override
    void MidiContinue();                                               //override

    void updateUI(Midi10UI *ui);
    bool IsLearning() const;

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

private:
    static const int Size = 2;

    MonoPVG4Out m_Out[Size];

    int m_LearnIdx;

    ClockSyncOut *m_ClockSyncOut;
};
