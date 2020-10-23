#include "cvfunctions.h"
#include "analogoutbank.h"
#include "DigitalOutBank.h"
#include "shiftoutbank.h"

void PitchOut(AnalogOutBank& out, int idx, uint8_t midiNote)
{
    //Assumes 1000 = 1Volt
    const uint32_t multiplier = 1000;
    const uint32_t divider = 12;
    uint32_t pitch = multiplier*midiNote/divider;
    const uint32_t maxPitch = 4095;
    if(maxPitch<pitch)
    {
        pitch = maxPitch;
    }
    out.set(idx, pitch);
}

void VelocityOut(AnalogOutBank& out, int idx, uint8_t velocity)
{
    // velocity 7 bits [0,127] out 12 bits (assumed!)
    int vel = velocity<<5;
    out.set(idx, vel);
}

void CCOut(AnalogOutBank& out, int idx, uint8_t value)
{
    // CC value 7 bits [0,127] out 12 bits (assumed!)
    int val = value<<5;
    out.set(idx, val);
}

void GateOut(DigitalOutBank& out, int idx, int gate)
{
    out.set(idx, gate ? HIGH : LOW);
}

void GateOut(ShiftOutBank& out, int idx, int gate)
{
    out.set(idx, gate ? HIGH : LOW);
}
