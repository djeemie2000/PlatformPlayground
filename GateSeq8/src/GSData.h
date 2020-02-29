#pragma once

#include <Arduino.h>

class GSId
{
public:
    static const uint8_t GSTrackId = 0x01;
    static const uint8_t GSCommonId = 0x02;
};


struct GSTrack
{
    //ID and version bytes for storage
    uint8_t m_ID;
    uint8_t m_Version;

    unsigned long m_Pattern;
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
void Copy(const GSPattern& source, GSPattern& dest);
bool Equals(const GSPattern& a, GSPattern& b);

struct GSBank
{
    static const int NumPatterns = 2;

    GSPattern m_Pattern[NumPatterns];
};

void Init(GSBank& bank, int NumSteps);

struct GSCommon
{
    //ID and version bytes for storage
    uint8_t m_Id;
    uint8_t m_Version;

    int m_SelectedPattern;
    int m_SelectedBank;
};

void Init(GSCommon& common);

struct GSMem
{
    static const int NumBanks = 1;

    GSCommon m_Common;
    GSBank m_Bank[NumBanks];
};

void Init(GSMem& mem, int NumSteps);
