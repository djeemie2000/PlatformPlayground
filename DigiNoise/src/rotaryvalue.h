#pragma once

#include "library/include/attiny.h"

class RotaryValue
{
public:
    RotaryValue() : m_Value(0), m_MinValue(-32000), m_MaxValue(32000), m_Increment(1)
    {}

    void begin(int minValue, int maxValue, int increment)
    {
        m_MinValue = minValue;
        m_MaxValue = maxValue;
        m_Increment = increment;
    }

    void reset(int value)
    {
        m_Value = value;
    }

    void update(int incr)
    {
        // incr == 0 => unchanged
        if(0<incr)
        {
            if(m_Value + m_Increment<m_MaxValue)
            {
                m_Value += m_Increment;
            }
        }
        else if(incr<0)
        {
            if(m_Value - m_Increment>m_MinValue)
            {
                m_Value -= m_Increment;
            }
        }
    }

    int get() const { return m_Value;}

private:
    int m_Value;

    int m_MinValue;
    int m_MaxValue;
    int m_Increment;
};