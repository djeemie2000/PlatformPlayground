#pragma once

#include <mbed.h>//uint32_t, ...

uint32_t BitRead(uint32_t value, uint32_t bit)
{
    return (value >> bit) & 0x01;
} 

void BitSet(uint32_t& value, uint32_t bit)
{
    value |= (1UL << bit);
} 

void BitClear(uint32_t& value, uint32_t bit)
{
    value &= (~(1UL << bit));
} 

void BitWrite(uint32_t& value, uint32_t bit, uint32_t bitValue)
{
    if(bitValue)
    {
        BitSet(value, bit);
    }
    else
    {
        BitClear(value, bit);
    }
} 

void BitInvert(uint32_t& value, uint32_t bit)
{
    if(BitRead(value, bit))
    {
        BitClear(value, bit);
    }
    else
    {
        BitSet(value, bit);
    }
} 
