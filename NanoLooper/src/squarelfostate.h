#pragma once
#include <Arduino.h>

template<int Size>
class SquareLFOState
{
public:
    SquareLFOState(){}

    void Begin()
    {
        for(int idx = 0; idx<Size; ++idx)
        {
            squareLFOCounter[idx] = 0;
            squareLFOState[idx] = 1;
            squareLFOPeriod[idx] = 8000;
        }
    }

    void SetPeriod(int idx, int period)
    {
        // no check on index
        squareLFOPeriod[idx] = period;
    }

    int Get(int idx) const
    {
        // no check on index
        return squareLFOState[idx];
    }

    void Reset()
    {
        for(int idx = 0; idx<Size; ++idx)
        {
            squareLFOCounter[idx] = 0;
            squareLFOState[idx] = 1;
        }
    }

    void Update()
    {
        for(int idx = 0; idx<Size; ++idx)
        {
            if(squareLFOPeriod[idx] < squareLFOCounter[idx])
            {            
                squareLFOCounter[idx] = 0;
                squareLFOState[idx] = 1 - squareLFOState[idx];
#ifdef DOSERIALDEBUG
                Serial.println(squareLFOPeriod);
#endif
            }
            ++squareLFOCounter[idx];            
        }
    }

private:
    // square LFO out (1/0)
    int squareLFOCounter[Size];
    int squareLFOPeriod[Size];
    int squareLFOState[Size];
};
