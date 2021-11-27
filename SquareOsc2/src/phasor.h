#pragma once
#include <mbed.h>

template<class T, int Scale, int Mode>
class Phasor
{
public:
    Phasor() 
    : m_MaxPhase(1<<Scale)
    , m_Phase(0)
    , m_Crossed(0)
    {}

    void tick(T delta)
    {
        m_Phase += delta;
        m_Crossed = 0;
        if(m_MaxPhase<=m_Phase)
        {
            //accurate phase
            if(Mode)
            { 
                m_Phase -= m_MaxPhase;
            }
            //or not accurate phase
            if(!Mode)
            {
                m_Phase = 0;
            }
            m_Crossed = 1;
        }
    }

    const T m_MaxPhase;
    
    T m_Phase;
    int m_Crossed;
};
