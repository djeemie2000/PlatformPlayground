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

static const int MaxNumDac = 3;
static const int Pin_ChipSelect[MaxNumDac] = {10, 9, 8};
static const byte PortMask_ChipSelect[MaxNumDac] = {0x04, 0x02, 0x01};//all pins are on PORTB !!!!

void Begin(int NumDac =1);
// {
//   for(int Dac = 0; Dac<NumDac && Dac<MaxNumDac; ++Dac)
//   {
//     pinMode(Pin_ChipSelect[Dac], OUTPUT);
//   }
//   SPI.begin();
//   SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
// }

void SetOutput(unsigned int Value, byte Channel, byte Gain, int Dac = 0);
// {
//   byte LowByte = Value & 0xff;
//   byte HighByte = ((Value >> 8) & 0xff) | Channel | Gain | Active;

//   //int pin = Pin_ChipSelect[Dac];
//   byte PortMask = PortMask_ChipSelect[Dac];
//   PORTB &= ~PortMask;//digitalWrite(Pin, LOW);
//   SPI.transfer(HighByte);
//   SPI.transfer(LowByte);
//   PORTB |= PortMask;// digitalWrite(Pin, HIGH); 
// } 

}
