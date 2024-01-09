#pragma once
#include <Arduino.h>
#include "mcp48xxdac.h"

class MCPDac
{
public:
    MCPDac(){}

    void Begin()
    {
        mcp48xxdac::Begin(1);
    }

    void Write(uint8_t value)
    {
        int val = value;
        mcp48xxdac::SetOutput(val<<4, mcp48xxdac::Channel_A, mcp48xxdac::Gain_x2, 0);
    }

    void WriteFlipped(uint8_t value)
    {
        int val = (value>>4) | (value<<4);//????
        mcp48xxdac::SetOutput(val<<4, mcp48xxdac::Channel_A, mcp48xxdac::Gain_x2, 0);
    }
};
