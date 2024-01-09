#pragma once
#include <Arduino.h>

void Degrade(uint8_t &value)
{
    if (0x7F < value)
    {
        --value;
    }
    else if (value < 0x7F)
    {
        ++value;
    }
}

void Saturate(uint8_t &value)
{
    // prevent flipping by ++ on 0xFF or -- on 0x00
    if (0x7F < value && value < 0xFF)
    {
        ++value;
    }
    else if (value < 0x7F && 0x00 < value)
    {
        --value;
    }
}

uint8_t Degrade(uint8_t value,  uint8_t degrade)
{
    // degrade should be <0x7F
    uint8_t degraded = 0x7F;

    if (0x7F+degrade < value)
    {
        degraded = value - degrade;
    }
    else if (value+degrade < 0x7F)
    {
        degraded = value + degrade;
    }

    return degraded;
}

uint8_t Saturate(uint8_t value, uint8_t saturate)
{
    // saturate should be <0x7F
    // prevent flipping
    uint8_t saturated = 0x7F;
    if (0x7F < value)
    {
        saturated = (value < 0xFF-saturate) ? (value + saturate) : 0xFF;
    }
    else if (value < 0x7F)
    {
        saturated = (saturate < value) ? (value - saturate) : 0x00;
    }
    return saturated;
}

// test for all -> iterate all uint8_t values,  serial print in vs out
void TestDegrade(int N)
{
    for(uint8_t value = 0x00; value<=0xFF; ++value)
    {
        Serial.print(value, HEX);
        Serial.print(' ');
        Serial.println(Degrade(value, N), HEX);
    }
}

void TestSaturate(int N)
{
    for(uint8_t value = 0x00; value<=0xFF; ++value)
    {
        Serial.print(value, HEX);
        Serial.print(' ');
        Serial.println(Saturate(value, N), HEX);
    }
}
