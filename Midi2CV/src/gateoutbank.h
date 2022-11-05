#pragma once
#include <Arduino.h>

class GateOutBank
{
public:
    static const int Size = 9;

    GateOutBank();

    void Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, 
                uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7,
                uint8_t pin8);

    void GateOn(int idx);
    void GateOff(int idx);
    void Trigger(int idx);

    void Update(uint8_t counter); // counter for triggers ?

    void PrintState();

private:
    uint8_t m_OutputPin[Size];
    uint8_t m_Gate[Size];
};

void AllGateOff(GateOutBank& bank);
void SetGate(GateOutBank& bank, int idx, int gate);
