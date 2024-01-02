#pragma once
#include <Arduino.h>

uint8_t BitCrush1(uint8_t value, uint8_t N)
{
    // N should be <=7 !!!
    uint8_t mask = (0xFF<<N);
    return value & mask;
}

uint8_t BitCrush2(uint8_t value, uint8_t N)
{
    // 0x0 1..1 0..0
    uint8_t mask = (0xFF<<(7-N)) & 0x7F;
    return value | mask;
}
