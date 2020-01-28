#pragma once

#include <mbed.h>

struct GSTrack
{
    uint32_t m_Pattern;
    int m_NumSteps;
    bool m_Muted;
    float m_GateDuration;
};

void Init(GSTrack& track, int NumSteps);

struct GSPattern
{
    static const int NumTracks = 8;

    GSTrack m_Track[NumTracks];
};

void Init(GSPattern& pattern, int NumSteps);

struct GSBank
{
    static const int NumPatterns = 8;

    GSPattern m_Pattern[NumPatterns];
};


