#pragma once
#include <Arduino.h>

// counter => bar, beat, tick (assuming 4 bars length)
uint16_t CounterToTick(uint16_t counter);
uint16_t CounterToBeat(uint16_t counter);
uint16_t CounterToBar(uint16_t counter);

struct TickerState
{
    uint16_t m_Tick;
    uint16_t m_Beat;
    uint16_t m_Bar;
    uint16_t m_NumBars;
};

void CounterToState(uint16_t counter, TickerState& state, uint16_t numBarShift = 2);

class MidiLooperTicker
{
public:
    MidiLooperTicker();

    void onTick(int clock, int reset);

    bool clockIsRising() const;
    bool clockIsFalling() const;

    uint16_t Counter(int divider) const;
    uint16_t recordingStep(int divider) const;
    void GetTickerState(int divider, TickerState& state, uint16_t numBarShift = 2);

private:
    int m_Clock;
    int m_PrevClock;

    uint16_t m_Counter;
};

