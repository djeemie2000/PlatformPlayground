#pragma once
#include "fastdac.h"
#include "mcpdac.h"

void SpeedTestFastDac(FastDac& dac, int analogInPin, int numRepeats = 5000)
{
    Serial.println("Speed test fast DAC...");

    uint32_t before = millis();
    int repeat = 0;
    while(repeat<numRepeats)
    {
        uint8_t audioValue = analogRead(analogInPin) >>4;
        dac.Write(audioValue);

        ++repeat;
    }
    uint32_t after = millis();
    uint32_t elapsed = after - before;

    Serial.print("Done : ");
    Serial.print(elapsed);
    Serial.println(" mSec");
}

void SpeedTestFastDac(MCPDac& dac, int analogInPin, int numRepeats = 5000)
{
    Serial.println("Speed test fast DAC...");

    uint32_t before = millis();
    int repeat = 0;
    while(repeat<numRepeats)
    {
        uint8_t audioValue = analogRead(analogInPin) >>4;
        dac.Write(audioValue);

        ++repeat;
    }
    uint32_t after = millis();
    uint32_t elapsed = after - before;

    Serial.print("Done : ");
    Serial.print(elapsed);
    Serial.println(" mSec");
}
