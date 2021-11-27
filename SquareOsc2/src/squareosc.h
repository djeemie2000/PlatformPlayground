#pragma once
#include <mbed.h>

struct SquareOsc
{
public:
  uint32_t phaseCntr;
  uint32_t pitchEnv;

  int oscOut;

  SquareOsc()
   : phaseCntr(0)
   , pitchEnv(0)
   , oscOut(0)
  {    
  }

  void tick(bool reset, uint32_t pitchPeriod, uint32_t pitchDecay)
  {
    //phasePeriod = period;

    if(reset)
    {
        pitchEnv = 0;
    }

    static const uint16_t MaxPitchPeriod = 512;//2048;
    uint16_t period = pitchPeriod + (pitchEnv >> 10);
    if(period>MaxPitchPeriod)
    {
        period = MaxPitchPeriod;
    }

    // reset does not affect oscOut i.e. no osc sync 
    ++phaseCntr;
    if(period<=phaseCntr)
    {
      phaseCntr = 0;
      oscOut = 1- oscOut;   
    }

    if(!reset)
    {
        pitchEnv += pitchDecay;
    }
  }
};
