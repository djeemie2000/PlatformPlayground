#pragma once

class ClockInState
{
public:
    ClockInState(int period = 500);

    void Tick(bool reset, bool updatePeriodUponReset = true);
    int Cntr() const;
    int Period() const;
    
private:
    int m_Cntr;
    int m_Period;
};

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