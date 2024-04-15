#pragma once
#include <Arduino.h>

template<int Size>
class TapeHead
{
public:
    TapeHead() 
    : m_Offset(0)
    {}

    void Reset()
    {
        m_Offset = 0;
    }

    void Reset(int offset)
    {
        m_Offset = offset;
    }

    int Get() const 
    {
        return m_Offset;
    }

    void Advance()
    {
        ++m_Offset;
        if(Size<=m_Offset)
        {
            m_Offset = 0;
        }
    }

    void Reverse()
    {
        --m_Offset;
        if(m_Offset<0)
        {
            m_Offset = Size - 1;
        }
    }

    void Reverse(int amount)
    {
        // assumes 0<=amount<Size
        m_Offset -= amount;
        if(m_Offset<0)
        {
            m_Offset += Size;
        }
    }


private:
    int m_Offset;
};
