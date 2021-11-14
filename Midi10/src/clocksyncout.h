#pragma once

#include <Arduino.h>
#include "MidiHandler.h"

class Midi10UI;

class ClockSyncHandler : public MidiHandler
{
public:
    ClockSyncHandler();

    void Begin(int idxClock, int idxReset, int idxClockLed, int idxResetLed, int idxClock2, int idxClock3);

    void MidiClock();
    void MidiContinue();

    void updateUI(Midi10UI *ui);

private:
    int m_IdxClock;
    int m_IdxClockLed;
    int m_IdxReset;
    int m_IdxResetLed;
    int m_IdxClock2;
    int m_IdxClock3;
    uint8_t m_Cntr;
};
