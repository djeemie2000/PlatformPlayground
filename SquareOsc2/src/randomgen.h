#pragma once
#include <mbed.h>

struct RandomGenUint16
{
  RandomGenUint16() 
  : value(0xF0F0)
  {}

  void begin(uint16_t seed = 0xFE89)
  {
    value = seed;
  }

  void next()
  {
    value = (value >> 0x01U) ^ (-(value & 0x01U) & 0xB400U);
  }

  uint16_t value;
};
