#include "TTP8229TouchPad.h"
#include <SPI.h>
#include <Wire.h>

TTP8229TouchPad::TTP8229TouchPad()
    : m_TouchState(0)
    , m_PrevTouchState(0)
{
}

void TTP8229TouchPad::Begin(int mode)
{
    m_Mode = mode;
    if(m_Mode==SPIMode)
    {
      // SPI uses pins 13=LED and 12 
      SPI.begin();
      SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE3));
    }
    else
    {
        Wire.begin();
    }
    delay(500); // Allow 500ms for the 8229BSF to get ready after turn-on
}

void TTP8229TouchPad::Read()
{
  m_PrevTouchState = m_TouchState;
  if(m_Mode == SPIMode)
  {
    // bit = 0 => touched, bit  1 => not touched
    m_TouchState = ~ SPI.transfer16(0);
    //TODO inverse bit order!!
  }
  else
  {
    m_TouchState = 0;
    Wire.requestFrom(0x57, 2, true);
    if (Wire.available()) 
    {
        m_TouchState = Wire.read() << 8;
        if (Wire.available()) 
        {
            m_TouchState |= Wire.read();
        }
    }
  }  
}

int TTP8229TouchPad::GetNumPads() const
{
    return 16;
}

bool TTP8229TouchPad::Get(int Pad) const
{
    Pad = 15-Pad;
    if(0<=Pad && Pad<GetNumPads())
    {
        return m_TouchState & (1<<Pad);
    }
    return false;
}

bool TTP8229TouchPad::IsPushed(int Pad) const
{
    Pad = 15-Pad;
	if(0<=Pad && Pad<GetNumPads())
    {
        return m_TouchState & (1<<Pad);
    }
    return false;
}

bool TTP8229TouchPad::IsClicked(int Pad) const
{
    Pad = 15-Pad;
    if(0<=Pad && Pad<GetNumPads())
    {
        return !(m_PrevTouchState & (1<<Pad)) && (m_TouchState & (1<<Pad));
    }
    return false;
}

bool TTP8229TouchPad::IsReleased(int Pad) const
{
    Pad = 15-Pad;
    if(0<=Pad && Pad<GetNumPads())
    {
        return (m_PrevTouchState & (1<<Pad)) && !(m_TouchState & (1<<Pad));
    }
    return false;
}
