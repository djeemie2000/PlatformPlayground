#pragma once
#include <Arduino.h>

class StepCounter
{
public :
    StepCounter();

    void Begin();//???
    void SetDivider(uint16_t divider);
    void SetLength(int length);

    void Tick();
    void Reset();

    int Step() const;
    bool Advance() const;

    void PrintState();

private:
    uint16_t m_Counter;
    uint16_t m_Divider;
    int m_Length;

    int m_Step;
    bool m_Advance;
};
