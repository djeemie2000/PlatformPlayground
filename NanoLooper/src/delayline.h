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
        m_Current = m_Values;
        m_End = m_Current + Size;
        memset(m_Values, 0x7F, Size);
    }

    void Advance()
    {
        ++m_Current;
        if(m_End<=m_Current)
        {
            m_Current = m_Values;
        }
    }

    void Reverse()
    {
        --m_Current;
        if(m_Current<m_Values)
        {
            m_Current = m_End - 1;
        }
    }

    void Reset()
    {
        m_Current = m_Values;
    }

    void SkipForward(int skip)
    {
        // skip should be 0 <= skip <= Size
        m_Current += skip;
        if(m_End<=m_Current)
        {
            m_Current -= Size;
        }

    }

    void SkipBackwards(int skip)
    {
        // skip should be 0 <= skip <= Size
        m_Current -= skip;
        if(m_Current<m_Values)
        {
            m_Current += Size;
        }

    }

    void Write(uint8_t value)
    {
        *m_Current = value;
    }

    uint8_t Read() const
    {
        return *m_Current;
    }

    void Degrade()
    {
        if(0x7F<*m_Current)
        {
            --*m_Current;
        }
        else if(*m_Current<0x7F)
        {
            ++*m_Current;
        }
    }

private:
    uint8_t m_Values[Size];
    uint8_t* m_Current;
    uint8_t* m_End;
};
