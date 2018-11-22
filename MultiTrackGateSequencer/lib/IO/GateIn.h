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

class DebounceState
{
public:
    DebounceState();
    DebounceState(int maximum);
    void SetMaximum(int maximum);

    void Tick(int Gate);
    int Get() const;

private:
    int m_State;
    int m_Cntr;
    int m_Maximum;
};

// class ToggleState
// {
// public:
//     GateState();

//     void Tick(int Gate);
//     int Get() const;
//     bool IsRising() const;
//     bool IsFalling() const;

// private:
//     int m_Curr;
//     int m_Prev;
//     GateState m_ToggleState;
// };

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

class ButtonIn
{
public:
    ButtonIn(PinName pin, int maximum);

    void Read();
    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

private:
    DigitalIn m_In;
    DebounceState m_DebounceState;
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

class Toggle3InOut
{
public:
    Toggle3InOut(PinName inPin, PinName outPin1, PinName outPin2);

    void Read();
    int Get() const;

private:
    static const int NumStates = 3;
    DigitalIn m_In;
    DigitalOut m_Out1;
    DigitalOut m_Out2;
    GateState m_State;
    int m_ToggleState;
};
