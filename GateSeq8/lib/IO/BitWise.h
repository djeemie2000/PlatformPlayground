#pragma once

#include <Arduino.h>//uint32_t, ...

template<class T>
T BitRead(T value, T bit)
{
    return (value >> bit) & 0x01;
} 

template<class T>
void BitSet(T& value, T bit)
{
    T mask = 1;
    value |= (mask << bit);
} 

template<class T>
void BitClear(T& value, T bit)
{
    T mask = 1;
    value &= (~(mask << bit));
} 

template<class T>
void BitWrite(T& value, T bit, T bitValue)
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

template<class T>
void BitInvert(T& value, T bit)
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
