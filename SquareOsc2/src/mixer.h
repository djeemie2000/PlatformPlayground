#pragma once
#include <mbed.h>

class Mixer
{
public:
        //TODO use LUT's? [0/1][0/1] 
        //or index = squareOsc.oscOut | (noiseOsc.oscOut<<1) | (mode<<2); oscOut = LUT[index];
    int mix2(int squareOscOut, int noiseOscOut, uint32_t mode)
    {
        // assumes oscOut's are 0 or 1
        int index = squareOscOut + (noiseOscOut<<1);//[0,3]
        // out = mode bit at index is set?
        return (mode >> index) & 1;
    }

    int mix(int squareOscOut, int noiseOscOut, uint32_t mode)
    {
        int oscOut = 0;

        if(mode==3)
        {
            // pure coloured noise
            oscOut = noiseOscOut;
        }
        else if(mode==2)
        {
            // mix square + coloured noise
            if(squareOscOut)
            {
                // square 1 and noise 1 => 1
                // square 1 and noise 0 => 0
                oscOut = noiseOscOut;
            }
            else
            {
                // square 0 and noise 1 => 0
                // square 0 and noise 0 => 1
                oscOut = (1-noiseOscOut);      
            }
            // ??? this is an xor : 1 if equal, 0 if not equal
        }
        else if(mode==1)
        {
            // mix square + coloured noise upon square on
            if(squareOscOut)
            {
                // square 1 and noise 1 => 1
                // square 1 and noise 0 => 0
                oscOut = noiseOscOut;
            }
            else
            {
                // square 0 and noise 1 => 0
                // square 0 and noise 0 => 0
                oscOut = 0;      
            }
            // this is an AND
        }
        else
        {
            // pure square
            oscOut = squareOscOut;
        }

        return oscOut;
    }


};