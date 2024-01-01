#pragma once
#include <Arduino.h>

template<int N>
class LowPassFilter
{
public:
    LowPassFilter() : m_Value(0)
    {}

    int Process(int value)
    {
        m_Value = ((m_Value<<N) - m_Value  + value)>>N;
        return m_Value;
    }

private:
    int m_Value;
};
