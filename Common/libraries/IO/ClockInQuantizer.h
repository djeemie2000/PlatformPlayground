#pragma once

#include <mbed.h>

class ClockInQuantizer
{
public:
    ClockInQuantizer(int numSegments);

    void Tick(bool reset, bool updatePeriodUponReset = true);
    int GetSegment() const;
    int GetNumSegments() const;
    
private:
    const int m_NumSegments;
    int m_Cntr;
    int m_Period;
};