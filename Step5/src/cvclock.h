#pragma once
#include <Arduino.h>

class CVClock
{
public:
    CVClock();

    void Begin();
    void Update(int clockIn, int cv);
    int Get() const;
private:
    // TODO counter, period...
    int m_State;
};
