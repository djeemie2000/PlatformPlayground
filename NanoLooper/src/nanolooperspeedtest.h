#pragma once
#include "fastdac.h"
#include "mcpdac.h"


void setupFastAnalogRead(int idx)
{
  // bits [2:0] of ADCSRA register determine clock divider
  if(idx<=0)
  {
      // by default it is 128 ~ 111
    ADCSRA &= (0xF8 | 0x07);
  }
  else if(1==idx)
  {
      // we can set it to 32 ~ 101
      ADCSRA &= (0xF8 | 0x05);
  }
  else if(2==idx)
  {
    // we can set it to 16 ~ 100
    ADCSRA &= (0xF8 | 0x04);
  }
  else
  {
      // we can set it to 8  ~ 011
      ADCSRA &= (0xF8 | 0x03);
  }
}

void SpeedTestNoDac(int analogInPin, int numRepeats = 5000)
{
    Serial.println("Speed test no DAC...");

    uint32_t before = millis();
    int repeat = 0;
    while(repeat<numRepeats)
    {
        uint8_t audioValue = analogRead(analogInPin) >>4;

        ++repeat;
    }
    uint32_t after = millis();
    uint32_t elapsed = after - before;

    Serial.print("Done : ");
    Serial.print(elapsed);
    Serial.println(" mSec");
}

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

void SpeedTestMCPDac(MCPDac& dac, int analogInPin, int numRepeats = 5000)
{
    Serial.println("Speed test MCP DAC...");

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
