#include "keypad.h"

KeyPad::KeyPad()
 : m_Debounce(5)
{
}

void KeyPad::Begin(int pinOutRow1, int pinOutRow2, int pinOutRow3, int pinOutRow4, 
int pinInCol1, int pinInCol2, int pinInCol3, int pinInCol4)
{
    m_PinOutRow[0] = pinOutRow1;
    m_PinOutRow[1] = pinOutRow2;
    m_PinOutRow[2]= pinOutRow3;
    m_PinOutRow[3] = pinOutRow4;

    m_PinInCol[0] = pinInCol1;
    m_PinInCol[1] = pinInCol2;
    m_PinInCol[2]= pinInCol3;
    m_PinInCol[3] = pinInCol4;

    for(int row = 0; row<NumRows; ++row)
    {
        pinMode(m_PinOutRow[row], OUTPUT);
    }
    for(int col = 0; col<NumCols; ++col)
    {
        pinMode(m_PinInCol[col], INPUT);
    }

    for(int pad= 0;pad<NumPads; ++pad)
    {
        m_TouchCntr[pad] = 0;
        m_TouchState[pad] = 0;
        m_PrevTouchState[pad] = 0;
    }
}

void KeyPad::Read()
{
    // m_PrevTouchState = m_TouchState;

    for(int row = 0; row<NumRows; ++row)
    {
        ReadRow(row);
    }
}

void KeyPad::ReadRow(int row)
{
    // no check on row!!
    digitalWrite(m_PinOutRow[row], HIGH);
    for(int col = 0; col<NumCols; ++col)
    {
        int pad = row * NumCols + col;
        int tmp = digitalRead(m_PinInCol[col]);

        // debounce on immediate value
        if(tmp)
        {
            if(m_TouchCntr[pad]<m_Debounce)
            {
                ++m_TouchCntr[pad];
            }
        }
        else if(0<m_TouchCntr[pad])
        {
            --m_TouchCntr[pad];
        }
        

        // touchstate to prev!
        m_PrevTouchState[pad] =  m_TouchState[pad];
        // set new touchState if needed
        if(0 == m_TouchCntr[pad])
        {
            m_TouchState[pad] = 0;
        }
        else if(m_Debounce == m_TouchCntr[pad])
        {
            m_TouchState[pad] = 1;
        }
    }
    digitalWrite(m_PinOutRow[row], LOW);
}

int KeyPad::GetNumPads() const
{
    return NumPads;
}

bool KeyPad::Get(int Pad) const
{
    if(0<=Pad && Pad<GetNumPads())
    {
        return m_TouchState[Pad];
    }
    return false;
}

bool KeyPad::IsPushed(int Pad) const
{
	if(0<=Pad && Pad<GetNumPads())
    {
        return m_TouchState[Pad];
    }
    return false;
}

bool KeyPad::IsClicked(int Pad) const
{
    if(0<=Pad && Pad<GetNumPads())
    {
        return (!m_PrevTouchState[Pad]) && m_TouchState[Pad];
    }
    return false;
}

bool KeyPad::IsReleased(int Pad) const
{
    if(0<=Pad && Pad<GetNumPads())
    {
        return m_PrevTouchState[Pad] && (!m_TouchState[Pad]);
    }
    return false;
}
