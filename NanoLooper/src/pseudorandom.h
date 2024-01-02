#pragma once
#include <Arduino.h>

class PseudoRandom
{
public:
    PseudoRandom()
    {
        Reset();
    }

    void Reset()
    {
        m_Random = 22222;
    }

    uint32_t Rand()
    {
        m_Random = (m_Random * 196314165) + 907633515;
        return m_Random;
    }

private:
    // Calculate pseudo-random 32 bit number based on linear congruential method.
    // http://www.musicdsp.org/showone.php?id=59
    // static unsigned long random = 22222;
    // random = (random * 196314165) + 907633515;
    uint32_t m_Random;
};
