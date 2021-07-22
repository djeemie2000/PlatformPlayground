#pragma once
#include <Arduino.h>

// // counter => bar, beat, tick (assuming 4 bars length)
// uint16_t CounterToTick(uint16_t counter);
// uint16_t CounterToBeat(uint16_t counter);
// uint16_t CounterToBar(uint16_t counter);

struct TickerState
{
    uint16_t m_Tick;
    uint16_t m_Beat;
    uint16_t m_Bar;
    uint16_t m_NumBars;
};

void CounterToState(uint16_t counter, TickerState& state, uint16_t numBarShift);

class MidiLooperTicker
{
public:
    MidiLooperTicker();

    void onTick(int clock);
    void SetNumBars(uint16_t numBarShift); // 0/1/2/3/4 -> 1/2/4/8/16 bars
    void ToggleNumBars(uint16_t minNumBarshift, uint16_t maxNumBarShift);
    void reset();

    bool clockIsRising() const;
    bool clockIsFalling() const;

    uint16_t Counter() const;
    uint16_t recordingStep() const;
    void GetTickerState(TickerState& state) const;

private:
    int m_Clock;
    int m_PrevClock;
    uint16_t m_Counter;
    bool m_Reset;

    uint16_t m_NumBarShift;
};

