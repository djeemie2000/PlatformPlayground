#pragma once
#include <Arduino.h>

class DualStepCounter
{
public:
    DualStepCounter();

    void Begin();//???
    void SetDividerA(uint16_t divider);
    void SetLengthA(int length);
    void SetDividerB(uint16_t divider);
    void SetLengthB(int length);

    void Tick();
    void Reset();

    int StepA() const;
    int StepB() const;
    // bool AdvanceA() const;
    // bool AdvanceB() const;

    void PrintState();

private:
    bool m_Chaining;
    bool m_EnableA;
    bool m_EnableB;

    uint16_t m_CounterA;
    uint16_t m_DividerA;
    int m_LengthA;
    int m_StepA;
    bool m_AdvanceA;

    uint16_t m_CounterB;
    uint16_t m_DividerB;
    int m_LengthB;
    int m_StepB;
    bool m_AdvanceB;
};
