#pragma once
#include <Arduino.h>

class MidiLooperTicker
{
public:
    MidiLooperTicker();

    void onTick(int clock, int reset);

    bool clockIsRising() const;
    bool clockIsFalling() const;

    uint16_t Counter(int divider) const;
    uint16_t recordingStep(int divider) const;

private:
    int m_Clock;
    int m_PrevClock;

    uint16_t m_Counter;
};

// counter => bar, beat, tick (assuming 4 bars length)
uint16_t CounterToTick(uint16_t counter);
uint16_t CounterToBeat(uint16_t counter);
uint16_t CounterToBar(uint16_t counter);

