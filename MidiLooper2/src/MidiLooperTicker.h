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

struct TickerState2
{
    static const int NumTicksPerBeat = 4;
    static const int NumBeatsPerBar = 4;

    int m_Counter;
    int m_Tick;
    int m_Beat;
    int m_Bar;

    int m_NumBars;

    TickerState2()
    : m_Counter(0)
    , m_Tick(0)
    , m_Beat(0)
    , m_Bar(0)
    , m_NumBars(8)
    {}

    void Tick()
    {
        ++m_Counter;
        // if(MaxCounter()<=m_Counter)
        // {
        //     m_Counter = 0;
        // }

        ++m_Tick;
        if(NumTicksPerBeat<=m_Tick)
        {
            m_Tick = 0;
            ++m_Beat;
            if(NumBeatsPerBar<=m_Beat)
            {
                m_Beat = 0;
                ++m_Bar;
                if(m_NumBars<=m_Bar)
                {
                    m_Bar = 0;
                    m_Counter = 0;
                }
            }
        }
    }

    void Reset()
    {
        m_Counter = 0;
        m_Tick = 0;
        m_Beat = 0;
        m_Bar = 0;
    }

    int MaxCounter() const{ return m_NumBars * NumBeatsPerBar * NumTicksPerBeat;}
};

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

