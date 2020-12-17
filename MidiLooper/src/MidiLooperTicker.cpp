#include "MidiLooperTicker.h"

MidiLooperTicker::MidiLooperTicker()
    : m_Clock(0), m_PrevClock(0), m_Counter(0), m_NumBarShift(2) {}

 void MidiLooperTicker::onTick(int clock, int reset)
{
    m_PrevClock = m_Clock;
    m_Clock = clock;
    if (clockIsRising())
    {
        //rising clock => augment counter
        ++m_Counter;
    }
}

void MidiLooperTicker::SetNumBars(uint16_t numBarShift)
{
    m_NumBarShift = numBarShift;
}

bool MidiLooperTicker::clockIsRising() const
{
    return !m_PrevClock && m_Clock;
}

bool MidiLooperTicker::clockIsFalling() const
{
    return m_PrevClock && !m_Clock;
}

uint16_t MidiLooperTicker::Counter(int divider) const
{
    // assumes 
    //  4 ticks per beat 
    //  4 beats per bar
    //  4 bars (length)
    uint16_t mask = (1u<<6)-1;
    return (m_Counter / divider)& mask;
}

uint16_t MidiLooperTicker::recordingStep(int divider) const
{
    // 'quantized' recording step:
    // clock on => use current step for recording
    // clock off => use next step for recording
    if(m_Clock)
    {
        return Counter(divider);
    }

    // assumes 
    //  4 ticks per beat 
    //  4 beats per bar
    //  4 bars (length)
    uint16_t mask = (1u<<6)-1;
    return (Counter(divider)+1)&mask;
}

void MidiLooperTicker::GetTickerState(int divider, TickerState& state) const
{
    CounterToState(m_Counter/divider, state, m_NumBarShift);
}

uint16_t CounterToTick(uint16_t counter)
{
    // assumes 4 ticks per beat
    return counter & 0x03;
}

uint16_t CounterToBeat(uint16_t counter)
{
    // assumes 4 ticks per beat
    return (counter >> 2) & 0x03;
}
uint16_t CounterToBar(uint16_t counter)
{
    // assumes 4 ticks per beat
    // assumes 4 beats per bar => mask 0x03 = (1<<2)-1;

    //assumes counter is cropped to # bars
    return (counter >> 4); //& 0x03;
}

void CounterToState(uint16_t counter, TickerState& state, uint16_t numBarShift)
{
    // assumes 4 ticks per beat
    state.m_Tick = counter & 0x03;
    // assumes 4 ticks per beat
    state.m_Beat =(counter >> 2) & 0x03;

    // assumes 4 ticks per beat
    // assumes 4 beats per bar
    // => shift >> 4
    uint16_t barMask = (1u<<numBarShift) -1;
    state.m_Bar =  (counter >> 4) & barMask;

    state.m_NumBars = 1u<<numBarShift;
}
