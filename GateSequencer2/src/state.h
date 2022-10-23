#pragma once
#include <Arduino.h>

// descriptor for a single pattern (8 tracks)
struct Pattern
{
  static const int NumTracks = 8;
  static const int NumSteps = 32;
  uint8_t steps[NumSteps];// 1 bit per track
  // 4 play/performance properties, 1 bit per track
  uint8_t playMute;
  uint8_t solo;// only 1 or zero solo at any time!
  uint8_t fill;
  uint8_t playProperty4;  
  // 8 track properties, 1 bit per track
  uint8_t clockOnValue;
  uint8_t clockOffValue;
  uint8_t gateTrigger;// gate=1 trigger = 0
  uint8_t trackLink;// 2 bits per 2 tracks , or / chain 2 tracks
  uint8_t trackProperty5;
  uint8_t trackProperty6;
  uint8_t trackProperty7;
  uint8_t trackProperty8;

  Pattern()
   : playMute(0xFF)
   , solo(0x00)
   , fill(0x00)
   , playProperty4(0x00)
   , clockOnValue(0xFF)
   , clockOffValue(0x00)
   , gateTrigger(0xFF)
   , trackLink(0x00)
   , trackProperty5(0x00)
   , trackProperty6(0x00)
   , trackProperty7(0x00)
   , trackProperty8(0x00)
  {
    for(int idx = 0; idx<NumSteps; ++ idx)
    {
      steps[idx] = 0x00;
    }
  }
};

int GetStep(Pattern* pattern, int track, int step)
{
  return bitRead(pattern->steps[step], track);
}

void SetStep(Pattern* pattern, int track, int step)
{
  bitSet(pattern->steps[step], track);
}

void ClearStep(Pattern* pattern, int track, int step)
{
  bitClear(pattern->steps[step], track);
}

void ToggleStep(Pattern* pattern, int track, int step)
{
  bitToggle(pattern->steps[step], track);
}

int IsPlaying(Pattern* pattern, int track)
{
  return bitRead(pattern->playMute, track);
}

void TogglePlayMute(Pattern* pattern, int track)
{
  bitToggle(pattern->playMute, track);
}

void ToggleClockOnValue(Pattern* pattern, int track)
{
  bitToggle(pattern->clockOnValue, track);
}

void ToggleClockOffValue(Pattern* pattern, int track)
{
  bitToggle(pattern->clockOffValue, track);
}

void ToggleGateTrigger(Pattern* pattern, int track)
{
  bitToggle(pattern->gateTrigger, track);
}

void ToggleSolo(Pattern* pattern, int track)
{
  // on to off?
  if(bitRead(pattern->solo, track))
  {
    pattern->solo = 0x00;
  }
  // or else set (only 1 solo at a time!)
  else 
  {
    pattern->solo = bit(track);
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
  int mode; // bit 0 edit mode, bit 1 performance mode, (bit 0 and 1 utility?), bit 2 and 3 for FA FB sub modes 
  // playing????

  State()
   : slot(0)
   , bank(0)
   , editTrack(0)
   , editQuadrant(0)
   , mode(1)//edit mode
  {
    // // default pattern for debugging/testing 
    // pattern[0].playMute = 0xFF;//all tracks playing?
    // pattern[0].link = 0x00;//no links
    // for(int step = 0; step<Pattern::NumSteps; ++step)
    // {
    //   pattern[0].steps[step] = step<<3;
    // }
  }
};

//shared state between interrupt and loop
struct SharedState
{
  Pattern* currentPattern;
  int currentStep;//[0,31]
  bool doReset;
  bool doAdvance;
  bool playing;

  SharedState()
   : currentPattern(0)
   , currentStep(0)
   , doReset(false)
   , doAdvance(false)
   , playing(true)
  {}
};

struct InterruptState
{
  int clockIn;
  int resetIn;
  //uint8_t clockCounter;//??
  //TODO trigger counter per track?
};
