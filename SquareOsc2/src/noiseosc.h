#pragma once
#include <mbed.h>

struct NoiseOsc
{
  int oscOut;
  uint16_t oscOutHistory;

  NoiseOsc() 
  : oscOut(0)
  , oscOutHistory(0)
  {}

  void tick(uint16_t randomValue, uint16_t noiseColor)
  {
    //TODO mask 0x01 is uncolored noise (?)
    if(noiseColor>=8)
    {
      // pure uncolored noise
      uint16_t mask = 0x01;
      oscOutHistory = oscOutHistory << 1 | (randomValue & 1);
      oscOut =  (oscOutHistory & mask) ? 1 : 0;
    }
    else
    {
      // coloured noise
      uint16_t mask = 0x00FF>>noiseColor;//[0,7]
//      oscOut =  (randomValue & mask) ? 1 : 0;
//      oscOutHistory = oscOutHistory << 1 | oscOut;
      oscOutHistory = oscOutHistory << 1 | (randomValue & 1);
      oscOut = (oscOutHistory & mask) ? 1 : 0;
    }
  }
};
