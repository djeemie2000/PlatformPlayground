#include "TTP8229TouchPad.h"
#include <SPI.h>
#include <Wire.h>

TTP8229TouchPad_I2C::TTP8229TouchPad_I2C()
    : m_TouchState(0)
    , m_PrevTouchState(0)
{
}

void TTP8229TouchPad_I2C::Begin()
{
    Wire.begin();
    delay(500); // Allow 500ms for the 8229BSF to get ready after turn-on
}

void TTP8229TouchPad_I2C::Read()
{
  m_PrevTouchState = m_TouchState;

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

int TTP8229TouchPad_I2C::GetNumPads() const
{
    return 16;
}

bool TTP8229TouchPad_I2C::Get(int Pad) const
{
    Pad = 15-Pad;
    if(0<=Pad && Pad<GetNumPads())
    {
        return m_TouchState & (1<<Pad);
    }
    return false;
}

bool TTP8229TouchPad_I2C::IsPushed(int Pad) const
{
    Pad = 15-Pad;
	if(0<=Pad && Pad<GetNumPads())
    {
        return m_TouchState & (1<<Pad);
    }
    return false;
}

bool TTP8229TouchPad_I2C::IsClicked(int Pad) const
{
    Pad = 15-Pad;
    if(0<=Pad && Pad<GetNumPads())
    {
        return !(m_PrevTouchState & (1<<Pad)) && (m_TouchState & (1<<Pad));
    }
    return false;
}

bool TTP8229TouchPad_I2C::IsReleased(int Pad) const
{
    Pad = 15-Pad;
    if(0<=Pad && Pad<GetNumPads())
    {
        return (m_PrevTouchState & (1<<Pad)) && !(m_TouchState & (1<<Pad));
    }
    return false;
}
