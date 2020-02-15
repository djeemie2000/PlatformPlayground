#pragma once

#include <Arduino.h>

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


class GateIn
{
public:
    GateIn(uint8_t pin, bool pullup);

    void Read();
    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

private:
    uint8_t m_InPin;
    GateState m_State;
};

class ButtonIn
{
public:
    ButtonIn(uint8_t pin, int maximum);

    void Read();
    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

private:
    uint8_t m_InPin;
    DebounceState m_DebounceState;
    GateState m_State;
};


class ToggleIn
{
public:
    ToggleIn(uint8_t pin);

    void Read();
    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

private:
    uint8_t m_InPin;
    GateState m_State;
    GateState m_ToggleState;
};




