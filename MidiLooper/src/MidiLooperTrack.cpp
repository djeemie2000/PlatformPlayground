#include "MidiLooperTrack.h"

void MidiLooperTicker::onTick(int clock, int reset)
{
    m_PrevClock = m_Clock;
    m_Clock = clock;
    if (clockIsRising())
    {
        //rising clock => augment counter
        ++m_Counter;
        if (m_MaxCounter <= m_Counter || reset)
        {
            m_Counter = 0;
        }
    }
}