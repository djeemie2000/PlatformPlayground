#include "MidiLooperClock.h"

MidiLooperClock::MidiLooperClock()
: m_ClockPeriod(125)
, m_ClockCounter(0)
, m_BPM(120)
{}

void MidiLooperClock::Tick(int bpmUnmapped)
{
    // 120 BPM -> 2Hz => 500 msec period / 4 ticks per beat = 125 msec
    // range from 60 BPM to 180 BPM => middle position = 120 BPM
    // -> 62.5 to 250 msec period
    const int minBPM = 60;
    const int maxBPM = 180;
    int clockBPM = map(bpmUnmapped, 0, 1023, minBPM, maxBPM);
    SetBPM(clockBPM);
    
    Tick();
}

void MidiLooperClock::Tick()
{
    if (m_ClockCounter < m_ClockPeriod)
    {
      ++m_ClockCounter;
    }
    else
    {
      m_ClockCounter = 0;
    }
}

    
void MidiLooperClock::SetBPM(int bpm)
{
    m_BPM = bpm;
    const int defaultBPM = 120;
    const int defaultPeriod = 125;
    m_ClockPeriod = defaultPeriod * defaultBPM / bpm;
}

int MidiLooperClock::GetBPM() const
{
    return m_BPM;
}

int MidiLooperClock::Get() const
{
    int clock = m_ClockCounter < (m_ClockPeriod / 2) ? 1 : 0;
    return clock;
}
