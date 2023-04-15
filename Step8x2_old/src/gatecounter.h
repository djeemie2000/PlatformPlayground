#pragma once
#include <Arduino.h>

class GateCounter
{
public:
    GateCounter();

    void Begin();//???
    void SetDivider(uint16_t divider);

    void Tick();
    void Reset();

    int Gate() const;
    // isAdvanced();???

    void PrintState();

private:
    uint16_t m_Counter;
    uint16_t m_Divider;

    int m_Gate;
};
