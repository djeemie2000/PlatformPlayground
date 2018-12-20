#include "Mcp4822VoltageOut.h"

Mcp4822VoltageOut::Mcp4822VoltageOut(MCP4x22* mcp, int channel)
 : m_Mcp(mcp)
 , m_Channel(channel?MCP4x22::Channel_B:MCP4x22::Channel_A)
{}

void Mcp4822VoltageOut::WriteValue(float value)
{
    int intValue = value * 4096;//full range corresponds to value 1.0f
    m_Mcp->SetOutput(intValue, m_Channel, MCP4x22::Gain_x2);
}

void Mcp4822VoltageOut::WriteVoltage(float voltage)
{
    int intValue = voltage * 1000;//gain x2 => 4096 mV corresponds with 4096 range
    m_Mcp->SetOutput(intValue, m_Channel, MCP4x22::Gain_x2);
}
