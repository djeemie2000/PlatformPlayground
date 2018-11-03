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

class ToggleIn
{
public:
    ToggleIn(PinName pin);

    void Read();
    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

private:
    DigitalIn m_In;
    GateState m_State;
    GateState m_ToggleState;
};

class ToggleInOut
{
public:
    ToggleInOut(PinName inPin, PinName outPin);

    void Read();
    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

private:
    DigitalIn m_In;
    DigitalOut m_Out;
    GateState m_State;
    GateState m_ToggleState;
};
