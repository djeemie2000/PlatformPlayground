#include "stepcounter.h"

StepCounter::StepCounter()
 : m_Counter(0)
, m_Divider(1)
, m_Length(8)
, m_Step(0)
{}

void StepCounter::Begin()
{    
    m_Counter = 0;
    m_Divider = 1;
    m_Length = 8;
    m_Step = 0;
    m_Advance = false;
}

void StepCounter::SetDivider(uint16_t divider)
{
    m_Divider = divider;
}

void StepCounter::SetLength(int length)
{
    m_Length = length;
}

void StepCounter::Tick()
{
    ++m_Counter;
    
    bool m_Advance = (m_Counter % m_Divider) == 0;
    if(m_Advance)
    {
        ++m_Step;
        if(m_Length<=m_Step)
        {
            m_Step = 0;
        }
    }
}

void StepCounter::Reset()
{
    m_Counter = 0;

    m_Step = 0;
    m_Advance = true;
}

int StepCounter::Step() const
{
    return m_Step;
}

bool StepCounter::Advance() const
{
    return m_Advance;
}

void StepCounter::PrintState()
{
    Serial.print("D ");
    Serial.print(m_Divider);
    Serial.print(" L ");
    Serial.print(m_Length);
    Serial.print(" S ");
    Serial.println(m_Step);
}
