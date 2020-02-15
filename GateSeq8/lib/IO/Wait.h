#pragma once

#include <Arduino.h>

void wait_ms(unsigned long msec)
{
    delay(msec);
}

void wait_us(unsigned long usec)
{
    delayMicroseconds(usec);
}
