#pragma once
#include <Arduino.h>

struct SquareOsc
{
public:
  uint16_t phaseCntr;
  uint32_t pitchEnv;

  int oscOut;

  SquareOsc()
   : phaseCntr(0)
   , pitchEnv(0)
   , oscOut(0)
  {    
  }

  void tick(bool reset, uint32_t pitchPeriod, uint16_t pitchDecay)
  {
    //phasePeriod = period;

    if(reset)
    {
        pitchEnv = 0;
    }

    static const uint16_t MaxPitchPeriod = 2048;
    uint16_t period = pitchPeriod + (pitchEnv >> 8);
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
        pitchEnv += pitchDecay;
    }
  }
};
