#pragma once
#include <Arduino.h>

// descriptor for a single pattern (8 tracks)
struct Pattern
{
  static const int NumTracks = 8;
  static const int NumSteps = 32;
  uint8_t steps[NumSteps];// 1 bit per track
  uint8_t playMute;// 1 bit per track
  uint8_t link;// 1 bit per track
};

int Get(Pattern* pattern, int track, int step)
{
  return bitRead(pattern->steps[step], track);
  //return (pattern.steps[step] & (1 << track)) ? 1 : 0;
}

void Set(Pattern* pattern, int track, int step)
{
  bitSet(pattern->steps[step], track);
//  (pattern.steps[step]) |= (1 << track);
}

void Clear(Pattern* pattern, int track, int step)
{
  bitClear(pattern->steps[step], track);
  //(pattern.steps[step]) &= ~(1 << track);
}

void Toggle(Pattern* pattern, int track, int step)
{
  if(Get(pattern, track, step))
  {
    Clear(pattern, track, step);
  }
  else
  {
    Set(pattern, track, step);
  }
}

// loop state
struct State
{
  // multiple banks?
  // EEPROM size limited to 512 byts so only 8 patterns = 1 bank possible
  static const int NumSlots = 8;
  static const int NumBanks = 1;
  static const int NumPatterns = NumSlots* NumBanks;

  Pattern pattern[NumPatterns];

  int slot;
  int bank;
  int editTrack;//[0-7]
  int editQuadrant; // [0-3]

  State()
   : slot(0)
   , bank(0)
   , editTrack(0)
   , editQuadrant(0)
  {
    // default pattern for debugging/testing 
    pattern[0].playMute = 0xFF;//all tracks playing?
    pattern[0].link = 0x00;//no links
    for(int step = 0; step<Pattern::NumSteps; ++step)
    {
      pattern[0].steps[step] = step<<3;
    }
  }
};

//shared state between interrupt and loop
struct SharedState
{
  Pattern* currentPattern;
  int currentStep;//[0,31]

  SharedState()
   : currentPattern(0)
   , currentStep(0)
  {}
};

struct InterruptState
{
  int clockIn;
  int resetIn;
  //uint8_t clockCounter;//??
};
