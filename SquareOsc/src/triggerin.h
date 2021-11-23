#pragma once
#include <Arduino.h>
#include <fastpinio.h>

template<int N>
class TriggerInPortD
{
public:
    int currGate;
    int prevGate;
    int triggered;
    int released;

    TriggerInPortD() 
    : currGate(0)
    , prevGate(0)
    , triggered(0)
    , released(0)
    {}

    void tick()
    {
        prevGate = currGate;
        currGate = FastPinGetPortD<N>();
        triggered = !prevGate&& currGate;
        released = prevGate&& !currGate;
    }
};