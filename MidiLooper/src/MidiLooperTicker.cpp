#include "MidiLooperTicker.h"

MidiLooperTicker::MidiLooperTicker(int maxCounter)
    : m_Clock(0), m_PrevClock(0), m_Counter(0), m_MaxCounter(maxCounter) {}

 void MidiLooperTicker::onTick(int clock, int reset)
{
    m_PrevClock = m_Clock;
    m_Clock = clock;
    if (clockIsRising())
    {
        //rising clock => augment/reset counter
        ++m_Counter;
        if (m_MaxCounter <= m_Counter || reset)
        {
            m_Counter = 0;
        }
    }
}

bool MidiLooperTicker::clockIsRising() const
{
    return !m_PrevClock && m_Clock;
}

bool MidiLooperTicker::clockIsFalling() const
{
    return m_PrevClock && !m_Clock;
}

int MidiLooperTicker::Counter(int divider) const
{
    return m_Counter / divider;
}

int MidiLooperTicker::recordingStep(int divider) const
{
    // 'quantized' recording step:
    // clock on => use current step for recording
    // clock off => use next step for recording
    uint8_t step = Counter(divider);
    if (!m_Clock)
    {
        ++step;
        if (m_MaxCounter <= step)
        {
            step = 0;
        }
    }
    return step;
}