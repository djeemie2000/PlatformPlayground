#include "mcp48xxdac.h"

namespace mcp48xxdac
{

void Begin(int NumDac)
{
  for(int Dac = 0; Dac<NumDac && Dac<MaxNumDac; ++Dac)
  {
    pinMode(Pin_ChipSelect[Dac], OUTPUT);
  }
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
}

void SetOutput(unsigned int Value, byte Channel, byte Gain, int Dac)
{
  byte LowByte = Value & 0xff;
  byte HighByte = ((Value >> 8) & 0xff) | Channel | Gain | Active;

  //int pin = Pin_ChipSelect[Dac];
  byte PortMask = PortMask_ChipSelect[Dac];
  PORTB &= ~PortMask;//digitalWrite(Pin, LOW);
  SPI.transfer(HighByte);
  SPI.transfer(LowByte);
  PORTB |= PortMask;// digitalWrite(Pin, HIGH); 
} 

}
