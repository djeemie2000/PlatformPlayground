#pragma once
#include <mbed.h>

#include "noiseosc.h"
#include "parameters.h"
#include "noiseosc.h"
#include "squareosc.h"
#include "gater.h"
#include "mixer.h"


class FullOsc
{
public:
    FullOsc(PinName gateInPin, PinName oscOutPin)
     : parameters()
     , gateIn(gateInPin, PullUp)
     , oscOut(oscOutPin)
     , gater()
     , squareOsc()
     , noiseOsc()
     , mixer()
    {}

    void tick(uint32_t randomValue)
    {
        int extGate = gateIn;
        gater.tick(false, extGate, parameters.gateOnPeriod);

        bool reset = (gater.gateCntr == 0);
        squareOsc.tick(reset, parameters.pitchPeriod, parameters.pitchDecay);

        noiseOsc.tick(randomValue, parameters.noiseColor);

        // mix sq with noise depending on parameters
        if(0 == gater.gate)
        {
            // output off outside gate on period
            oscOut = 0;
        }
        else
        {    
            // gate is on
            oscOut = mixer.mix(squareOsc.oscOut, noiseOsc.oscOut, parameters.mode);
        }
    }

    Parameters parameters;
    DigitalIn gateIn;
    DigitalOut oscOut;
    Gater gater;
    SquareOsc squareOsc;
    NoiseOsc noiseOsc;
    Mixer mixer;//stateless

};
