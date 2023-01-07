#pragma once
#include <Arduino.h>


struct MidiLooperTickerParams
{
    // 24 PPQ to ticks
    int period24PPQ;
    int on24PPQ;

    // ticker to beats/bars
    int ticksPerBeat;
    int beatsPerBar;
    int numBars;

    MidiLooperTickerParams()
     : period24PPQ(6)
     , on24PPQ(3)
     , ticksPerBeat(4)
     , beatsPerBar(4)
     , numBars(8)
    {
        //defaults
    }
};

class MidiLooperTicker
{
public:
    MidiLooperTicker();

    void Begin(MidiLooperTickerParams params);

    void Tick();
    void Reset();

    bool GetClockOn() const { return m_ClockOn;    }
    int GetTick() const { return m_Tick;    }
    int GetBeat() const { return m_Beat;    }
    int GetBar() const { return m_Bar;  }

    int GetStep() const { return m_Step;    }
    // TODO next step
    int GetNumSteps() const { return m_NumSteps; }

private:
    bool m_DoReset;
    int m_Cntr24PPQ;// is this needed??

    int m_CntrTick;

    bool m_ClockOn;
    int m_Tick;
    int m_Beat;
    int m_Bar;

    int m_Step;
    int m_NumSteps;

    MidiLooperTickerParams m_Params;
};
