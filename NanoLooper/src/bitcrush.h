#pragma once
#include <Arduino.h>

uint8_t BitCrush1(uint8_t value, uint8_t N)
{
    // set lsb's to zero
    // keep bit7 to 1 always to preserve sign!
    // N should be <=7 !!!
    uint8_t mask = (0xFF<<N);
    return value & mask;
}

uint8_t BitCrush1b(uint8_t value, uint8_t N)
{
    // set lsb's to one
    // keep bit7 to 1 always to preserve sign!
    // N should be <=7 !!!
    uint8_t mask = (1<<N)-1;
    return value | mask;
}

uint8_t BitCrush2(uint8_t value, uint8_t N)
{
    // set msb's to one 
    // keep bit7 to 1 always to preserve sign!
    // 0x0 1..1 0..0
    uint8_t mask = (0xFF<<(7-N)) & 0x7F;
    return value | mask;
}

uint8_t BitCrush3(uint8_t value, uint8_t N)
{
    // set single bit to zero
    // keep bit7 to 1 always to preserve sign!
    // N should be <=7 !!!
    return bitClear(value, N);
}

uint8_t BitCrush4(uint8_t value, uint8_t mask)
{
    // shape/mask value according to mask
//    // keep bit7 to 1 always to preserve sign!
//    uint8_t msk = mask | 0x70;
    return value & mask;
}

//TODO test for all -> iterate all uint8_t values,  serial print in vs out
void TestBitCrush1(int N)
{
    for(uint8_t value = 0x00; value<=0xFF; ++value)
    {
        Serial.print(value, HEX);
        Serial.print(' ');
        Serial.println(BitCrush1(value, N), HEX);
    }
}

void TestBitCrush1b(int N)
{
    for(uint8_t value = 0x00; value<=0xFF; ++value)
    {
        Serial.print(value, HEX);
        Serial.print(' ');
        Serial.println(BitCrush1b(value, N), HEX);
    }
}

void TestBitCrush2(int N)
{
    for(uint8_t value = 0x00; value<=0xFF; ++value)
    {
        Serial.print(value, HEX);
        Serial.print(' ');
        Serial.println(BitCrush2(value, N), HEX);
    }
}

void TestBitCrush3(int N)
{
    for(uint8_t value = 0x00; value<=0xFF; ++value)
    {
        Serial.print(value, HEX);
        Serial.print(' ');
        Serial.println(BitCrush3(value, N), HEX);
    }
}

void TestBitCrush4(int N)
{
    for(uint8_t value = 0x00; value<=0xFF; ++value)
    {
        Serial.print(value, HEX);
        Serial.print(' ');
        Serial.println(BitCrush4(value, N), HEX);
    }
}
