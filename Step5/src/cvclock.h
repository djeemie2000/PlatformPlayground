#pragma once
#include <Arduino.h>
#include "cvclockstate.h"

class CVClock
{
public:
    CVClock();

    void Begin();
    void Update(int clockIn, int cv);
    int Get() const;
private:
// int m_State;
    CVClockState m_State;
};
