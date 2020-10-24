#include "analogoutbank.h"
#include "mcp48xxdac.h"

AnalogOutBank::AnalogOutBank(int size)
 : m_Size(size)
{
}

void AnalogOutBank::begin()
{
    int numDac = (1+m_Size)/2;
    mcp48xxdac::Begin(numDac);
}

void AnalogOutBank::set(int idx, int value)
{
    if (0 <= idx && idx < Capacity)
    {
        m_Values[idx] = value;
    }
}

void AnalogOutBank::update()
{
    for (int idx = 0; idx<m_Size && idx < Capacity; ++idx)
    {
        int dac = idx/2;
        int channel = idx%2;
        mcp48xxdac::SetOutput(m_Values[idx], channel?mcp48xxdac::Channel_B:mcp48xxdac::Channel_A, mcp48xxdac::Gain_x2, dac);
    }
}

void testAnalogOutBank(AnalogOutBank &bank, int repeats)
{
    for (int repeat = 0; repeat < repeats || repeats < 0; ++repeat)
    {
        for (int idx = 0; idx < bank.size(); ++idx)
        {
            for(int val = 0; val<=4096; val+=256)
            {
                bank.set(idx, val);
                bank.update();

                Serial.print("Set ");
                Serial.print(idx);
                Serial.print(" to ");
                Serial.println(val);
                delay(200);
            }
            bank.set(idx, 0);
            bank.update();
         }
    }
}
