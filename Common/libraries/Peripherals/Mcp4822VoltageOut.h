#pragma once

#include <mbed.h>
#include "VoltageOut.h"
#include "MCP4x22.h"

class Mcp4822VoltageOut : public VoltageOut
{
public:
    Mcp4822VoltageOut(MCP4x22* mcp, int channel);// 0 => channel A, 1 => channel B

    void WriteValue(float Value) /*override*/;
    void WriteVoltage(float voltage) /*override*/;

private:
    MCP4x22* m_Mcp;
    int m_Channel;
};
