#include "bitbeattrack.h"
#include "digitaloutbank.h"
#include "ledoutbank.h"
#include "EEPROM.h"

BitBeatTrack::BitBeatTrack() : m_NumSteps(0x00), m_DoReset(false), m_Step(0x00), m_PlayMute(true), m_GateOn(false)
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

void BitBeatTrack::Play(bool clockRising, bool clockFalling, bool resetRising, bool resetClicked, 
                            bool btn50On, bool btnTrackClicked)
{
    if(btn50On)
    {
        // toggle play mute if track btn clicked
        if(btnTrackClicked)
        {
            m_PlayMute = !m_PlayMute;
        }
    }

    if(0==m_NumSteps)
    {
        m_GateOn = false;
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

        m_GateOn =  m_PlayMute && (GetBitOn(m_Step)?true:false);
    }

    if (clockFalling)
    {
        m_GateOn = m_PlayMute && (GetBitOff(m_Step)?true:false);
    }
}

void BitBeatTrack::StartRecording()
{
    //TODO state
    m_NumSteps = 0;
    m_GateOn = 0;
    // keep play mute?    
}

void BitBeatTrack::StopRecording()
{
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

void BitBeatTrack::DisplayPlay(LedOutBank& ledOut, int idx)
{
    // gate + led ~ state
    if(m_GateOn && m_PlayMute)
    {
        ledOut.LedOn(idx);
    }
    else
    {
        ledOut.LedOff(idx);
    }
}

void BitBeatTrack::DisplayPlay(DigitalOutBank& gateOut, int idx)
{
    // gate ~ state
    if(m_GateOn && m_PlayMute)
    {
        gateOut.Set(idx);
    }
    else
    {
        gateOut.Clear(idx);
    }
}

void BitBeatTrack::DisplayPlayMute(LedOutBank& ledOut, int idx)
{
    // led ~~play mute
    if(m_PlayMute)
    {
        ledOut.LedOn(idx);
    }
    else
    {
        ledOut.LedOff(idx);
    }
}

void BitBeatTrack::DisplayRecording(DigitalOutBank& gateOut, LedOutBank& ledOut, int idx)
{
    gateOut.Clear(idx);
    ledOut.LedBlink(idx);
}

int BitBeatTrack::ParamSize() const
{
    return 3 + 2*ByteCapacity;
}

void BitBeatTrack::SaveParams(int offset)
{
    // Serial.print(offset);
    // Serial.println("Save ");

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
    // Serial.print(offset);
    // Serial.println("Load ");

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

