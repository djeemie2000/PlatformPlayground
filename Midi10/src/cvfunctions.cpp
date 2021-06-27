#include "cvfunctions.h"
#include "analogoutbank.h"
#include "DigitalOutBank.h"
#include "ledoutbank.h"

void PitchOut(AnalogOutBank &out, int idx, uint8_t midiNote, uint8_t baseNote)
{
    uint8_t note = baseNote < midiNote ? midiNote - baseNote : 0x00;
    //Assumes 1000 = 1Volt
    const uint32_t multiplier = 1000;
    const uint32_t divider = 12;
    uint32_t pitch = multiplier * note / divider;
    const uint32_t maxPitch = 4095;
    if (maxPitch < pitch)
    {
        pitch = maxPitch;
    }
    out.set(idx, pitch);
}

void VelocityOut(AnalogOutBank &out, int idx, uint8_t velocity)
{
    // velocity 7 bits [0,127] out 12 bits (assumed!)
    int vel = velocity << 5;
    out.set(idx, vel);
}

void CCOut(AnalogOutBank &out, int idx, uint8_t value)
{
    // CC value 7 bits [0,127] out 12 bits (assumed!)
    int val = value << 5;
    out.set(idx, val);
}

// void GateOut(AnalogOutBank& out, int idx, int gate)
// {
//     out.set(idx, gate ? 4095 : 0);
// }

// void GateOut(DigitalOutBank& out, int idx, int gate)
// {
//     out.set(idx, gate ? HIGH : LOW);
// }

// void LedOut(LedOutBank& out, int idx, int gate)
// {
//     out.set(idx, gate);
// }
