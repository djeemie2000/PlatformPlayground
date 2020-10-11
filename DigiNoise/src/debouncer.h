#pragma once

#include "library/include/attiny.h"


class Debouncer
{
public:
    Debouncer() : m_Value(false),m_ChangeCntr(0), m_Debounce(5){}

    void begin(uint8_t debounce = 5)
    {
        m_Value = false;
        m_ChangeCntr = 0;
        m_Debounce = debounce;
    }

    void debounce(bool value)
    {
        //debounce
        if(value != m_Value)
        {
            ++m_ChangeCntr;
            if(m_Debounce<=m_ChangeCntr)
            {
                //toggle m_Value
                m_Value = value;
                m_ChangeCntr = 0;
            }
        }
        else if(0<m_ChangeCntr)
        {
            --m_ChangeCntr;
        }
    }

    bool Get() const{return m_Value;}

private:
    bool m_Value;
    uint8_t m_ChangeCntr;
    uint8_t m_Debounce;
};
