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

    volatile int m_Period;
    volatile int m_Cntr;
    volatile bool m_DoReset;
    volatile bool m_Flag;
};
