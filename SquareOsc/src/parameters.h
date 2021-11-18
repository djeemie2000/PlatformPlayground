#pragma once
#include <Arduino.h>

struct Parameters
{

  uint16_t gatePeriod;

  uint16_t mode;//SQ, mix, Noise

  uint16_t pitchPeriod;
  uint16_t pitchDecay;
  uint16_t noiseColor;
  uint32_t gateOnPeriod;

  Parameters()
   : gatePeriod(2048)
   , mode(0)
   , pitchPeriod(64)
   , pitchDecay(0)
   , noiseColor(0)
   , gateOnPeriod(8192)
  {}

};
