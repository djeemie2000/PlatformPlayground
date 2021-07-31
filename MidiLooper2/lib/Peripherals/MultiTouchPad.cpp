#include "MultiTouchPad.h"

MultiTouchPad::MultiTouchPad()
{
    m_Size = 0;
    memset(m_TouchPad, 0, sizeof(m_TouchPad));
    m_Size2 = 0;
    memset(m_TouchPad2, 0, sizeof(m_TouchPad2));
}

void MultiTouchPad::Add(MPR121TouchPad* touchPad)
{
    m_TouchPad[m_Size++] = touchPad;
}

void MultiTouchPad::Add(TTP8229TouchPad_I2C* touchPad)
{
    m_TouchPad2[m_Size2++] = touchPad;
}

void MultiTouchPad::Begin()
{
}

void MultiTouchPad::Read()
{
    for(int idx = 0; idx<m_Size; ++idx)
    {
        m_TouchPad[idx]->Read();
    }
    for(int idx = 0; idx<m_Size2; ++idx)
    {
        m_TouchPad2[idx]->Read();
    }
}

int MultiTouchPad::GetNumPads() const
{
    int numPads = 0;
    for(int idx = 0; idx<m_Size; ++idx)
    {
        if(m_TouchPad[idx] != nullptr)
        {
            numPads += m_TouchPad[idx]->GetNumPads();
        }
    }
    for(int idx = 0; idx<m_Size2; ++idx)
    {
        if(m_TouchPad2[idx] != nullptr)
        {
            numPads += m_TouchPad2[idx]->GetNumPads();
        }
    }

    return numPads;
}

bool MultiTouchPad::Get(int Pad) const
{
    if(Pad<0)
    {
        return false;
    }

    int offset = 0;
    for(int idx = 0; idx<m_Size; ++idx)
    {
        if(Pad < offset + m_TouchPad[idx]->GetNumPads())
        {
            return m_TouchPad[idx]->Get(Pad-offset);
        }
        offset += m_TouchPad[idx]->GetNumPads();
    }
    for(int idx = 0; idx<m_Size2; ++idx)
    {
        if(Pad < offset + m_TouchPad2[idx]->GetNumPads())
        {
            return m_TouchPad2[idx]->Get(Pad-offset);
        }
        offset += m_TouchPad2[idx]->GetNumPads();
    }
    
    return false;
}

bool MultiTouchPad::IsPushed(int Pad) const
{
    if(Pad<0)
    {
        return false;
    }

    int offset = 0;
    for(int idx = 0; idx<m_Size; ++idx)
    {
        if(Pad < offset + m_TouchPad[idx]->GetNumPads())
        {
            return m_TouchPad[idx]->IsPushed(Pad-offset);
        }
        offset += m_TouchPad[idx]->GetNumPads();
    }
    for(int idx = 0; idx<m_Size2; ++idx)
    {
        if(Pad < offset + m_TouchPad2[idx]->GetNumPads())
        {
            return m_TouchPad2[idx]->IsPushed(Pad-offset);
        }
        offset += m_TouchPad2[idx]->GetNumPads();
    }

    return false;
}

bool MultiTouchPad::IsClicked(int Pad) const
{
    if(Pad<0)
    {
        return false;
    }

    int offset = 0;
    for(int idx = 0; idx<m_Size; ++idx)
    {
        if(Pad < offset + m_TouchPad[idx]->GetNumPads())
        {
            return m_TouchPad[idx]->IsClicked(Pad-offset);
        }
        offset += m_TouchPad[idx]->GetNumPads();
    }
    for(int idx = 0; idx<m_Size2; ++idx)
    {
        if(Pad < offset + m_TouchPad2[idx]->GetNumPads())
        {
            return m_TouchPad2[idx]->IsClicked(Pad-offset);
        }
        offset += m_TouchPad2[idx]->GetNumPads();
    }
    
    return false;
}

bool MultiTouchPad::IsReleased(int Pad) const
{
    if(Pad<0)
    {
        return false;
    }

    int offset = 0;
    for(int idx = 0; idx<m_Size; ++idx)
    {
        if(Pad < offset + m_TouchPad[idx]->GetNumPads())
        {
            return m_TouchPad[idx]->IsReleased(Pad-offset);
        }
        offset += m_TouchPad[idx]->GetNumPads();
    }
    for(int idx = 0; idx<m_Size2; ++idx)
    {
        if(Pad < offset + m_TouchPad2[idx]->GetNumPads())
        {
            return m_TouchPad2[idx]->IsReleased(Pad-offset);
        }
        offset += m_TouchPad2[idx]->GetNumPads();
    }
    
    return false;
}
