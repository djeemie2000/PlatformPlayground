#include "mcp4822bank.h"
#include "mcp48xxdac.h"

MCP4822Bank::MCP4822Bank()
{}

void MCP4822Bank::Begin()
{
    mcp48xxdac::Begin();
}

void MCP4822Bank::SetValue(int channel, int value)
{
    int dac = channel >> 1;
    int ch = (channel & 1) ? mcp48xxdac::Channel_B : mcp48xxdac::Channel_A;
    mcp48xxdac::SetOutput(value, ch, mcp48xxdac::Gain_x2, dac);
} 

void TestMCP4822Bank(MCP4822Bank& bank, int repeats)
{
    Serial.println("Testing mcp4822 bank...");
    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        for(int channel = 0;channel<MCP4822Bank::NumChannels; ++ channel)
        {
            for(int value = 0; value < 4096; value += 512)
            {
                Serial.print(channel);
                Serial.print(" ");
                Serial.println(value);

                bank.SetValue(channel, value);

                delay(250);
            }
        }
    }
}
