#pragma once
#include <mbed.h>

struct Parameters
{
  uint32_t mode;//SQ, mix, Noise

  uint32_t pitchPeriod;
  uint32_t pitchDecay;

  uint32_t noiseColor;
  
  uint32_t gateOnPeriod;

  Parameters()
   : mode(0)
   , pitchPeriod(64)
   , pitchDecay(0)
   , noiseColor(0)
   , gateOnPeriod(8192)
  {}

};
