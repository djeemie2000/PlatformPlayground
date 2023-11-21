#pragma once
#include <Arduino.h>

class MCP4822Bank
{
public:
    static const int NumChannels = 2;

    MCP4822Bank();

    void Begin();

    void SetValue(int channel, int value); // value 12 bits!

private:
    // no values, apply immediately upon call
};

void TestMCP4822Bank(MCP4822Bank& bank, int repeats);
