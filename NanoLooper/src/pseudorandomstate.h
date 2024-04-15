#pragma once
#include <Arduino.h>

#include "pseudorandom.h"

template<int Size>
class PseudoRandomState
{
public:
    PseudoRandomState(){}

    void Begin()
    {        
        pseudoRandom.Reset();
        for(int idx = 0; idx<Size; ++idx)
        {
            randomState[idx] = 0;
            randomThreshold[idx] = 128;
        }
    }

    void SetThreshold(int idx, uint16_t threshold)
    {
        // no check on index
        randomThreshold[idx] = threshold;
    }

    int Get(int idx) const
    {
        // no check on index
        return randomState[idx];
    }

    void Reset()
    {
        pseudoRandom.Reset();
        for(int idx = 0; idx<Size; ++idx)
        {
            randomState[idx] = 0;
        }
    }

    void Update()
    {
        uint32_t random = pseudoRandom.Rand();
        // compare with threshold ~ CV/pot
        for(int idx = 0; idx<Size; ++idx)
        {
            if ((random & 0xFFFF) < randomThreshold[idx])
            {
                // toggle state
                randomState[idx] = 1 - randomState[idx];
            }
        }
    }

private:
    // random out (1/0)
    PseudoRandom pseudoRandom;
    int randomState[Size];
    uint16_t randomThreshold[Size];
};
