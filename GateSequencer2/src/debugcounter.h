#pragma once
#include <Arduino.h>

class DebugCounter
{
public:
    DebugCounter()
    : m_DebugCounter(0)
    , m_DebugPeriod(1000)
    , m_PrevMillis(0)
    {}

    void Begin(int period)
    {
        m_DebugCounter =0;
        m_DebugPeriod = period;
        m_PrevMillis =0;
    }

    bool Tick(unsigned long currMillis,unsigned long& elapsedMillis)
    {
        bool ticked = false;

        elapsedMillis = currMillis - m_PrevMillis;

        ++m_DebugCounter;
        if(m_DebugPeriod<=m_DebugCounter)
        {
            m_DebugCounter = 0;
            m_PrevMillis = currMillis;
            ticked = true;
        }

        return ticked;
    }

    int GetPeriod() const {  return m_DebugPeriod;}

private:
    int m_DebugCounter;
    int m_DebugPeriod;
    unsigned long m_PrevMillis;
};
