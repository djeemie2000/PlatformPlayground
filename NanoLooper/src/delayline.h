#pragma once
#include <Arduino.h>

template<int Size>
class DelayLine
{
public:
    DelayLine()
    {}

    void Begin()
    {
        memset(m_Values, 0x7F, Size);
    }

    uint8_t Read(int offset) const
    {
        return *(m_Values + offset);
    }

    void Write(int offset, uint8_t value)
    {
        *(m_Values + offset) = value;
    }

private:
    uint8_t m_Values[Size];
};
