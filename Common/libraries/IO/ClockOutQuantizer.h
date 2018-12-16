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
