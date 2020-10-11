#pragma once

#include "library/include/attiny.h"

class RotaryEncoder
{
public:
    RotaryEncoder() : m_History(0x00)
    {}

    int encode(bool EncA, bool EncB)
    {
        uint8_t current = 0x00;
        if(EncA)
        {
            current |= 0x01;
        }
        if(EncB)
        {
            current |= 0x02;
        }
        if(current != (m_History&0x03))
        {
            m_History = (m_History<<2) | current;
            //changed
            static int tbl[16] =
            { 0, +1, -1, 0,
                // position 3 = 00 to 11, can't really do anythin, so 0
                -1, 0, 0/*-2*/, +1,
                // position 2 = 01 to 10, assume a bounce, should be 01 -> 00 -> 10
                +1, 0/*+2*/, 0, -1,
                // position 1 = 10 to 01, assume a bounce, should be 10 -> 00 -> 01
                0, -1, +1, 0
                // position 0 = 11 to 10, can't really do anything
            };

            return tbl[m_History&0x0f];
        }
        return 0;
    }

private:
    uint8_t m_History;
};
