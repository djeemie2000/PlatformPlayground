#pragma once

#include <Arduino.h>

class Timer
{
public:
    Timer();

    void start()
    {
        m_StartMillis = millis();
        m_ElapsedMillis = 0;
    }

    void stop()
    {
        m_ElapsedMillis = millis()-m_StartMillis;
    }

    void reset()
    {
        m_ElapsedMillis = 0;
    }

    unsigned long read_ms() const 
    {
        return m_ElapsedMillis;
    }

private:
    unsigned long m_StartMillis;
    unsigned long m_ElapsedMillis;
    //TODO running?
};
