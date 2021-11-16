#pragma once
#include <Arduino.h>

struct SquareOsc
{
public:
  uint16_t phaseCntr;
  uint16_t pitchEnv;

  int oscOut;

  SquareOsc()
   : phaseCntr(0)
   , pitchEnv(0)
   , oscOut(0)
  {    
  }

  void tick(bool reset, uint16_t pitchPeriod, uint16_t pitchDecay)
  {
    //phasePeriod = period;

    if(reset)
    {
        pitchEnv = 0;
    }

    static const uint16_t MaxPitchPeriod = 256;
    uint16_t period = pitchPeriod + (pitchEnv >> 9);
    if(period>MaxPitchPeriod)
    {
        period = MaxPitchPeriod;
    }

    // reset does not affect oscOut i.e. no osc sync 
    ++phaseCntr;
    if(period<=phaseCntr)
    {
      phaseCntr = 0;
      oscOut= 1- oscOut;   
    }

    if(!reset)
    {
        // check against overflow of pitchEnv
        static const uint16_t MaxPitchEnv = 16000;
        if(pitchEnv<MaxPitchEnv-pitchDecay)
        {
            pitchEnv += pitchDecay;
        }
    }
  }
};
