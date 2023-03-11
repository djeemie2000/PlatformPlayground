#include "bitbeattrack.h"
#include "digitaloutbank.h"
#include "ledoutbank.h"
#include "EEPROM.h"

BitBeatTrack::BitBeatTrack() : m_NumSteps(0x00), m_DoReset(false), m_Step(0x00)
{
    // bits all zero
    for(int idx = 0; idx<ByteCapacity; ++idx)
    {
        m_BitOn[idx] = 0x00;
        m_BitOff[idx] = 0x00;
    }
}

int BitBeatTrack::GetBitOn(uint8_t step) const
{
    return bitRead(m_BitOn[step>>3], step&0x07);
}

int BitBeatTrack::GetBitOff(uint8_t step) const
{
    return bitRead(m_BitOff[step>>3], step&0x07);
}

void BitBeatTrack::SetBitOn(uint8_t step)
{
    bitSet(m_BitOn[step>>3], step&0x07);
}

void BitBeatTrack::ClearBitOn(uint8_t step)
{
    bitClear(m_BitOn[step>>3], step&0x07);
}

void BitBeatTrack::SetBitOff(uint8_t step)
{
    bitSet(m_BitOff[step>>3], step&0x07);
}

void BitBeatTrack::ClearBitOff(uint8_t step)
{
    bitClear(m_BitOff[step>>3], step&0x07);
}

void BitBeatTrack::Play(bool clockRising, bool clockFalling, bool resetRising, bool resetClicked, DigitalOutBank &gateOut, LedOutBank &ledOut, int idx)
{
    if(0==m_NumSteps)
    {
        gateOut.Clear(idx);
        ledOut.LedOff(idx);
        return;
    }

    if (resetRising || resetClicked)
    {
        m_DoReset = true; // raise reset flag
    }

    if (clockRising)
    {
        // advace or reset step
        if (m_DoReset)
        {
            m_Step = 0x00;
            m_DoReset = false;
        }
        else
        {
            ++m_Step;
            if (m_NumSteps <= m_Step)
            {
                m_Step = 0x00;
            }
        }

        if (GetBitOn(m_Step))
        {
            gateOut.Set(idx);
            ledOut.LedOn(idx);
        }
        else
        {
            gateOut.Clear(idx);
            ledOut.LedOff(idx);
        }
    }

    if (clockFalling)
    {
        if (GetBitOff(m_Step))
        {
            gateOut.Set(idx);
            ledOut.LedOn(idx);
        }
        else
        {
            gateOut.Clear(idx);
            ledOut.LedOff(idx);
        }
    }
}

void BitBeatTrack::StartRecording(DigitalOutBank& gateOut, LedOutBank& ledOut, int idx)
{
    gateOut.Clear(idx);
    ledOut.LedBlink(idx);

    //TODO state
    m_NumSteps = 0;
}

void BitBeatTrack::StopRecording(DigitalOutBank& gateOut, LedOutBank& ledOut, int idx)
{
    // gate already off
    gateOut.Clear(idx);
    ledOut.LedOff(idx);

    m_DoReset = true;//trigger reset upon first clock after recording
}

void BitBeatTrack::Record(bool btn0Clicked, bool btn50Clicked, bool btn100Clicked)
{
    if (m_NumSteps < MaxNumSteps)
    {
        if (btn0Clicked)
        {
            ClearBitOn(m_NumSteps);
            ClearBitOff(m_NumSteps);
            ++m_NumSteps;
        }
        else if (btn50Clicked)
        {
            SetBitOn(m_NumSteps);
            ClearBitOff(m_NumSteps);
            ++m_NumSteps;
        }
        else if (btn100Clicked)
        {
            SetBitOn(m_NumSteps);
            SetBitOff(m_NumSteps);
            ++m_NumSteps;
        }
    }
}

int BitBeatTrack::ParamSize() const
{
    return 3 + 2*ByteCapacity;
}

void BitBeatTrack::SaveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'B');
    EEPROM.update(off++, 'T');
    EEPROM.update(off++, m_NumSteps);
    for(int idx = 0; idx<ByteCapacity; ++idx)
    {
        EEPROM.update(off++, m_BitOn[idx]);
    }
    for(int idx = 0; idx<ByteCapacity; ++idx)
    {
        EEPROM.update(off++, m_BitOff[idx]);
    }
}

void BitBeatTrack::LoadParams(int offset)
{
    int off = offset;
    if ('B' == EEPROM.read(off++) && 'T' == EEPROM.read(off++))
    {
        m_NumSteps = EEPROM.read(off++);
        for(int idx = 0; idx<ByteCapacity; ++idx)
        {
            m_BitOn[idx] = EEPROM.read(off++);
        }
        for(int idx = 0; idx<ByteCapacity; ++idx)
        {
            m_BitOff[idx] = EEPROM.read(off++);
        }
    }
}

