#pragma once
//#include <Arduino.h>

class Select
{
public:
    Select() : m_Curr(0), m_Prev(0) {}

    void Set(int select)
    {
        m_Prev = m_Curr;
        m_Curr = select;
    }

    int Get() const
    {
        return m_Curr;
    }

    int Previous() const
    {
        return m_Prev;
    }

private:
    int m_Curr;
    int m_Prev;
};
