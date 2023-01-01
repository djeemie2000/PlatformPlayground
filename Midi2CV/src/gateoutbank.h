#pragma once
#include <Arduino.h>

class DigitalOutBank;
class MCP4822Bank;

class GateOutBank
{
public:
    static const int Size = 8;

    GateOutBank();

    void Begin();

    void GateOn(int idx);
    void GateOff(int idx);
    void Trigger(int idx);

    void Update(uint8_t counter); // counter for triggers
    void Apply(DigitalOutBank& bank);
    void Apply(int offset, DigitalOutBank& bank, MCP4822Bank& abank);

    void PrintState();

private:
    uint8_t m_Gate[Size];//TODO uint8_t m_State;
    uint8_t m_Counter;
};

void AllGateOff(GateOutBank& bank);
void SetGate(GateOutBank& bank, int idx, int gate);
