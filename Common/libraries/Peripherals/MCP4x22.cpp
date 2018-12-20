#include "MCP4x22.h"

MCP4x22::MCP4x22(SPI* spi, PinName cs)
 : m_SPI(spi)
 , m_ChipSelect(cs, 1)
{
  m_SPI->format(16);//16 bits per write
}

void MCP4x22::SetOutput(int Value, int Channel, int Gain)
{
  // trunc value to [0,4095] !
  if(Value<0) {
    Value = 0;
  } else if(4095<Value)
  {
    Value = 4095;
  }
  int writeValue = (Value&0x03FF) | Channel | Gain | Active;
  m_ChipSelect.write(0);
  m_SPI->write(writeValue);
  m_ChipSelect.write(1);
}
