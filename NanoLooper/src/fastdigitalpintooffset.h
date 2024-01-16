#pragma once
#include <Arduino.h>

int PinToOffset(int pin)
{
    // offset order is port B (0..7) port C (8..15) port D (16..23)
    if (pin >= 2 && pin <= 7)
    {
        // PD2 .. PD7
        return 16 + pin;
    }
    if (pin >= 8 && pin <= 13)
    {
        // PB0 .. PB5
        return pin - 8; // 0 + pin - 8
    }
    if (pin >= 14 && pin <= 19)
    {
        // PC0 .. PC5
        return pin - 6; // 8 + pin - 14
    }
    return -1; // TODO
}
