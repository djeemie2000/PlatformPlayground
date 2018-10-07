#pragma once

#include <mbed.h>

class GateState
{
public:
    GateState();

    void Tick(int Gate);
    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

private:
    int m_Curr;
    int m_Prev;
};

//also: ButtonIn
class GateIn
{
public:
    GateIn(PinName pin);

    void Read();
    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

private:
    DigitalIn m_In;
    GateState m_State;
};
