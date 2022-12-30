#pragma once
#include <Arduino.h>

class PeriodicTicker
{
public:
    PeriodicTicker();

    bool Peek() const { return m_Flag; }
    void Clear()
    {
        m_Flag = false;
    }

    void Tick();
    void Reset() 
    {
        m_DoReset = true;
    }

    void SetPeriod(int period)
    {
        m_Period = period;
    }

private:
    void Set()
    {
        m_Flag = true;
    }

    int m_Period;
    int m_Cntr;
    bool m_DoReset;
    bool m_Flag;
};
