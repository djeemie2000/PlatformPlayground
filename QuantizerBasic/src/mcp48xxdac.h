#pragma once

#include <Arduino.h>
#include <SPI.h>

namespace mcp48xxdac
{

static const byte Gain_x1 = 1<<5;
static const byte Gain_x2 = 0x00;
static const byte Active = 1<<4;
static const byte Channel_A = 0x00;
static const byte Channel_B = 1<<7;

static const int MaxNumDac = 4;
static const int Pin_ChipSelect[MaxNumDac] = {8, 9};

void Begin()
{
  for(int dac = 0; dac<MaxNumDac; ++dac)
  {
    pinMode(Pin_ChipSelect[dac], OUTPUT);
  }
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
}

void ChipSelectLow(int dac)
{
  if(dac==0)
  {
    PORTB &= ~0x01;//PB0
  }
}

void ChipSelectHigh(int dac)
{
  if(dac == 0)
  {
    PORTB |= 0x01;//PB0
  }
}

void SetOutput(unsigned int value, byte channel, byte gain, int dac)
{
  byte LowByte = value & 0xff;
  byte HighByte = ((value >> 8) & 0xff) | channel | gain | Active;

  //int pin = Pin_ChipSelect[Dac];
  ChipSelectLow(dac);
  SPI.transfer(HighByte);
  SPI.transfer(LowByte);
  ChipSelectHigh(dac);
} 

}
