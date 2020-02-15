#pragma once

#include <Arduino.h>

class GateHandler
{
public:
    GateHandler(){}
    virtual void On() {}
    virtual void Off() {}
};

class DigitalOutGateHandler : public GateHandler
{
public:
    DigitalOutGateHandler(uint8_t outPin);

    void On();
    void Off();

private:
    uint8_t m_OutPin;
};
