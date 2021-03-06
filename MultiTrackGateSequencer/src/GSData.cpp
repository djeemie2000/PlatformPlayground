#include "GSData.h"

void Init(GSTrack& track, int NumSteps)
{
    track.m_Pattern = 0x00000000;
    track.m_NumSteps = NumSteps<=32 ? NumSteps : 32;
    track.m_Muted = false;
    track.m_GateDuration = 0.5f;
}

void Init(GSPattern& pattern, int NumSteps)
{
    for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
    {
        Init(pattern.m_Track[idx], NumSteps);
    }
}
