#pragma once
#include <Arduino.h>

struct Gater
{
  uint16_t gateCntr;
  int gate;

  Gater() 
  : gateCntr(0)
  , gate(0)
  {}

  void tick(uint16_t gatePeriod, uint16_t gateOnPeriod)
  {
    ++gateCntr;
    if(gatePeriod<gateCntr)
    {
      // reset
      gateCntr = 0;
      gate = 1;
    }
    
    gate = (gateOnPeriod<=gateCntr) ? 0 : 1;
  }
};
