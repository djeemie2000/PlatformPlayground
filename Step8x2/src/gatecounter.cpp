#include "gatecounter.h"

GateCounter::GateCounter()
 : m_Counter(0)
, m_Divider(1)
, m_Gate(1)
{}

void GateCounter::Begin()
{    
    m_Counter = 0;
    m_Divider = 1;
    m_Gate = 1;
}

void GateCounter::SetDivider(uint16_t divider)
{
    m_Divider = divider;
}

void GateCounter::Tick()
{
    ++m_Counter;

    uint8_t dividedGateCounter = m_Counter / m_Divider;
    m_Gate = 1-(dividedGateCounter & 1);
}

void GateCounter::Reset()
{
    m_Counter = 0;

    int dividedGateCounter = m_Counter / m_Divider;
    m_Gate = 1-(dividedGateCounter & 1);
}

int GateCounter::Gate() const
{
    return m_Gate;
}

// isAdvanced();???

void GateCounter::PrintState()
{
    Serial.print("D ");
    Serial.print(m_Divider);
    Serial.print(" C ");
    Serial.print(m_Counter);
    Serial.print(" G ");
    Serial.println(m_Gate);
}
