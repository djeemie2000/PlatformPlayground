#pragma once
#include <Arduino.h>

class FakeClock
{
public:
    FakeClock();
    void Begin(int period);
    void Tick();
    bool IsClicked() const;
    int Get() const;

private:
    int m_Counter;
    int m_Period;
};
