#include "MultiTouchPad.h"

MultiTouchPad::MultiTouchPad()
{
    memset(m_TouchPad, 0, sizeof(m_TouchPad));
}

void MultiTouchPad::Begin(MPR121TouchPad* touchPad1, MPR121TouchPad* touchPad2, MPR121TouchPad* touchPad3)
{
    m_TouchPad[0] = touchPad1;
    m_TouchPad[1] = touchPad2;
    m_TouchPad[2] = touchPad3;
}

void MultiTouchPad::Read()
{
    for(int idx = 0; idx<Capacity; ++idx)
    {
        if(m_TouchPad[idx] != nullptr)
        {
            m_TouchPad[idx]->Read();
        }
    }
}

int MultiTouchPad::GetNumPads() const
{
    int numPads = 0;
    for(int idx = 0; idx<Capacity; ++idx)
    {
        if(m_TouchPad[idx] != nullptr)
        {
            numPads += m_TouchPad[idx]->GetNumPads();
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
    for(int idx = 0; idx<Capacity; ++idx)
    {
        if(m_TouchPad[idx] != nullptr)
        {
            if(Pad < offset + m_TouchPad[idx]->GetNumPads())
            {
                return m_TouchPad[idx]->Get(Pad-offset);
            }
            offset += m_TouchPad[idx]->GetNumPads();
        }
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
    for(int idx = 0; idx<Capacity; ++idx)
    {
        if(m_TouchPad[idx] != nullptr)
        {
            if(Pad < offset + m_TouchPad[idx]->GetNumPads())
            {
                return m_TouchPad[idx]->IsPushed(Pad-offset);
            }
            offset += m_TouchPad[idx]->GetNumPads();
        }
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
    for(int idx = 0; idx<Capacity; ++idx)
    {
        if(m_TouchPad[idx] != nullptr)
        {
            if(Pad < offset + m_TouchPad[idx]->GetNumPads())
            {
                return m_TouchPad[idx]->IsClicked(Pad-offset);
            }
            offset += m_TouchPad[idx]->GetNumPads();
        }
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
    for(int idx = 0; idx<Capacity; ++idx)
    {
        if(m_TouchPad[idx] != nullptr)
        {
            if(Pad < offset + m_TouchPad[idx]->GetNumPads())
            {
                return m_TouchPad[idx]->IsReleased(Pad-offset);
            }
            offset += m_TouchPad[idx]->GetNumPads();
        }
    }
    
    return false;
}
