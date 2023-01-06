#include "midilooperticker.h"

MidiLooperTicker::MidiLooperTicker()
 : m_DoReset(true)
 , m_Cntr24PPQ(0)
 , m_CntrTick(0)
 , m_ClockOn(true)
 , m_Tick(0)
 , m_Beat(0)
 , m_Bar(0)
{

}

void MidiLooperTicker::Begin(MidiLooperTickerParams params)
{
    m_Params = params;

    m_DoReset = true;
}

void MidiLooperTicker::Tick()
{
    if(m_DoReset)
    {
        m_Cntr24PPQ = 0;
        m_CntrTick = 0;
        m_ClockOn = true;
        m_Tick = 0;
        m_Beat = 0;
        m_Bar = 0;
        m_DoReset = false;
    }
    else
    {
        // update 24PPQ counter
        ++m_Cntr24PPQ;
        if(24<=m_Cntr24PPQ)
        {
            m_Cntr24PPQ = 0;
        }

        // derive clockon, advance tick if needed
        bool advanceTick = false;
        ++m_CntrTick;
        if(m_Params.period24PPQ<=m_CntrTick)
        {
            m_CntrTick = 0;
            advanceTick = true;
        }
        m_ClockOn = (m_CntrTick<m_Params.on24PPQ);

        if(advanceTick)
        {
            ++m_Tick;
            if(m_Params.ticksPerBeat<=m_Tick)
            {
                m_Tick = 0;

                ++m_Beat;
                if(m_Params.beatsPerBar<=m_Beat)
                {
                    m_Beat = 0;

                    ++m_Bar;
                    if(m_Params.numBars<=m_Bar)
                    {
                        m_Bar = 0;
                    }
                }
            }
        }
    }
}

void MidiLooperTicker::Reset()
{
    m_DoReset = true;
}
