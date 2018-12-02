#pragma once

#include <mbed.h>

class NPositionIn
{
public:
    NPositionIn(PinName pin, int NumPositions);

    void Read();
    int Get() const;//return position in [0,NumPositions-1]

private:
    AnalogIn m_In;
    const int m_NumPositions;
    int m_Position;
};
