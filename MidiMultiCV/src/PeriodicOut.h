#pragma once

class PeriodicOutState
{
public:
    PeriodicOutState();

    void Tick(bool Reset);

    int Get() const;

    void SetPeriod(int Period);
    void SetDuration(float Duration);
    void Reset();
private:
    int m_Period;
    int m_Cntr;
    float m_Duration;
    int m_State;
};
