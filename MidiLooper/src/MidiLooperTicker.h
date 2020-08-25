#pragma once
#include <Arduino.h>

class MidiLooperTicker
{
public:
    MidiLooperTicker(int maxCounter = 64);

    void onTick(int clock, int reset);

    bool clockIsRising() const;
    bool clockIsFalling() const;

    int Counter(int divider) const;
    int recordingStep(int divider) const;

private:
    int m_Clock;
    int m_PrevClock; //??
    int m_Counter;
    int m_MaxCounter;
};