#include "GSData.h"
//#include <string.h>

void Init(GSTrack& track, int NumSteps)
{
    track.m_ID = GSId::GSTrackId;
    track.m_Version = 0x01;
    // payload
    track.m_Pattern = 0x00000000;
    track.m_NumSteps = NumSteps<=32 ? NumSteps : 32;
    track.m_Muted = false;
    track.m_GateDuration = 0.5f;
}

bool Equals(const GSTrack& a, GSTrack& b)
{
    return a.m_ID == b.m_ID 
    && a.m_Version == b.m_Version 
    && a.m_Pattern==b.m_Pattern 
    && a.m_NumSteps == b.m_NumSteps 
    && a.m_Muted == b.m_Muted 
    && a.m_GateDuration==b.m_GateDuration;
}

void Init(GSPattern& pattern, int NumSteps)
{
    for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
    {
        Init(pattern.m_Track[idx], NumSteps);
    }
}

void Init(GSBank& bank, int NumSteps)
{
    for(int idx = 0; idx<GSBank::NumPatterns; ++idx)
    {
        Init(bank.m_Pattern[idx], NumSteps);
    }
}

void Copy(const GSPattern& source, GSPattern& dest)
{
    memcpy(&dest, &source, sizeof(GSPattern));
}

bool Equals(const GSPattern& a, GSPattern& b)
{
    for(int track = 0; track<GSPattern::NumTracks; ++track)
    {
        if(!Equals(a.m_Track[track],b.m_Track[track]))
        {
            return false;
        }
        // if(0!=memcmp(a.m_Track[track], b.m_Track[track], sizeof(GSTrack)))
        // {
        //     return false;
        // }
    }
    return true;
    //return 0==memcmp(&a, &b, sizeof(GSPattern));
}

void Init(GSCommon& common)
{
    common.m_Id = GSId::GSCommonId;
    common.m_Version = 0x01;
    //payload
    common.m_SelectedPattern = 0;
    common.m_SelectedBank = 0;
}

void Init(GSMem& mem, int NumSteps)
{
    Init(mem.m_Common);
    for(int idx = 0; idx<GSMem::NumBanks; ++idx)
    {
        Init(mem.m_Bank[idx], NumSteps);
    }
}
