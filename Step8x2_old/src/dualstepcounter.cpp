#include "dualstepcounter.h"

DualStepCounter::DualStepCounter()
 : m_Chaining(false)
 ,m_EnableA(true)
 ,m_EnableB(true)
 ,m_CounterA(0)
, m_DividerA(1)
, m_LengthA(8)
, m_StepA(0)
, m_AdvanceA(false)
 ,m_CounterB(0)
, m_DividerB(1)
, m_LengthB(8)
, m_StepB(0)
, m_AdvanceB(false)
{}

void DualStepCounter::Begin()
{    
    //TODO
    // m_Counter = 0;
    // m_Divider = 1;
    // m_Length = 8;
    // m_Step = 0;
    // m_Advance = false;
}

void DualStepCounter::SetDividerA(uint16_t divider)
{
    m_DividerA = divider;
}

void DualStepCounter::SetDividerB(uint16_t divider)
{
    m_DividerB = divider;
}

void DualStepCounter::SetLengthA(int length)
{
    m_LengthA = length;
}

void DualStepCounter::SetLengthB(int length)
{
    m_LengthB = length;
}

void DualStepCounter::Tick()
{
    if(m_EnableA)
    {
        ++m_CounterA;
    }
    if(m_EnableB)
    {
        ++m_CounterB;
    }

    bool m_AdvanceA = (m_CounterA % m_DividerA) == 0;
    bool endOfSequenceA = m_AdvanceA && (m_StepA +1 >= m_LengthA);

    bool m_AdvanceB = (m_CounterB % m_DividerB) == 0;
    bool endOfSequenceB = m_AdvanceB && (m_StepB +1 >= m_LengthB);
           
      if(m_Chaining)
      {
        if(m_EnableA && m_AdvanceA)
        {
            if(endOfSequenceA)
            {
                m_EnableA = 0;
                m_EnableB = 1;
            }
        }
        else if(m_EnableB && m_AdvanceB)
        {
            if(endOfSequenceB)
            {
              m_EnableA = 1;
              m_EnableB = 0;
            }
        }
      }
      else
      {
        //not chaining
        if(m_EnableA && m_AdvanceA)
        {
            if(endOfSequenceB)
            {
                m_EnableA = 1;
                m_EnableB = 1;
            }
        }
        if(m_EnableB && m_AdvanceB)
        {
            if(endOfSequenceB)
            {
              m_EnableA = 1;
              m_EnableB = 1;
            }
        }
      }
}

void DualStepCounter::Reset()
{
    m_CounterA = 0;
    m_CounterB = 0;
    m_StepA = 0;
    m_StepB = 0;
    m_AdvanceA = true;
    m_AdvanceB = true;
}

int DualStepCounter::StepA() const
{
    return m_StepA;
}

// bool DualStepCounter::AdvanceA() const
// {
//     return m_AdvanceA;
// }

void DualStepCounter::PrintState()
{
    //TODO
    // Serial.print("D ");
    // Serial.print(m_Divider);
    // Serial.print(" L ");
    // Serial.print(m_Length);
    // Serial.print(" S ");
    // Serial.println(m_Step);
}
