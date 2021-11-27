#pragma once
#include <mbed.h>

struct Gater
{
  uint32_t gateCntr;
  int prevExtGate;
  int gate;

  Gater() 
  : gateCntr(0)
  , prevExtGate(0)
  , gate(0)
  {}

  void tick(bool retrigger, int extGate, uint32_t gateOnPeriod)
  {
    // off -> on : only upon rising extGate (if gateOnOeriod >0)
    // on -> off : only once, only when gate is already on, upon gateCntr > gateOnPeriod or extGate off

    if((!prevExtGate && extGate) || retrigger)
    {
      //Serial.println('R');

      // extGate rising => reset
      gateCntr = 0;
      gate = (gateCntr<gateOnPeriod);// if zero, stays at gate off
    }
    else
    {
      // no reset
      //if(gateCntr<16000)//prevent overflow,alternative : use uint32_t ?
      {
        ++gateCntr;
      }
      if(gate)
      {
        if(gateOnPeriod<gateCntr || !extGate)
        {
            gate = 0;
        }
      }    
    }

    prevExtGate = extGate;
  }
};
