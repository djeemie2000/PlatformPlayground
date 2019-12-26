#pragma once

#include <mbed.h>

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
    DigitalOutGateHandler(DigitalOut& out);

    void On();
    void Off();

private:
    DigitalOut& m_Out;
};
