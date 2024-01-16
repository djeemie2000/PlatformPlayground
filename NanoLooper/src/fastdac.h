#pragma once
#include <Arduino.h>

class FastDac
{
public:
    FastDac(){}

    void Begin()
    {
        // pins 4..7, 8..11
        // PD4..PD7, PB0..PB3 
        for(int pin = 4; pin<=11; ++pin)
        {
            pinMode(pin, OUTPUT);
        }
    }

    void Write(uint8_t value)
    {
        // 4 LSB to PD4..PD7,
        PORTD = (PORTD & 0x0F) | (value<<4);
        // 4 MSB to PB0..PB3
        PORTB = (PORTB & 0xF0) | (value>>4);
    }

    void WriteFlipped(uint8_t value)
    {
        // 4 MSB to PD4..PD7,
        PORTD = (PORTD & 0x0F) | (value>>4);
        // 4 LSB to PB0..PB3
        PORTB = (PORTB & 0xF0) | (value<<4);
    }
};


