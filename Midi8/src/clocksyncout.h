#pragma once

#include <Arduino.h>

class Midi8UI;

class ClockSyncOut
{
public:
    ClockSyncOut();

    void Begin(int idxClock, int idxReset);

    void MidiClock();
    void MidiContinue();

    void updateUI(Midi8UI* ui);

    
private:
    int m_IdxClock;
    int m_IdxReset;
    uint8_t m_Cntr;
};
