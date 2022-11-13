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
static const int Pin_ChipSelect[MaxNumDac] = {10, 9, A5, A4};

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
    PORTB &= ~0x04;//PB2
  }
  else if(dac == 1)
  {
    PORTB &= ~0x02;//PB1
  }
  else if(dac == 2)
  {
    PORTC &= ~0x20;//PC5
  }
  else if(dac == 3)
  {
    PORTC &= ~0x10;//PC4
  }
}

void ChipSelectHigh(int dac)
{
  if(dac == 0)
  {
    PORTB |= 0x04;//PB2
  }
  else if(dac == 1)
  {
    PORTB |= 0x02;//PB1
  }
  else if(dac == 2)
  {
    PORTC |= 0x20;//PC5
  }
  else if(dac == 3)
  {
    PORTC |= 0x10;//PC4
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
