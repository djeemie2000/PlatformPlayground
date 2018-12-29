#pragma once

#include "GateIn.h"

class ClockOutQuantizer
{
public:
    ClockOutQuantizer();

    void Tick(int segment);

    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

    void SetDuration(int duration);

private:
    int m_Duration;
    GateState m_State;
};

class ClockOutState
{
public:
    ClockOutState();

    void Tick(int cntr, int period);

    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

    void SetDuration(float duration);

private:
    float m_Duration;
    GateState m_State;
};
