#include "MemController.h"


bool LoadGSTrack(MemoryBank& memBank, int bank, int offset, GSTrack& track)
{
    uint8_t header[2];
    if(2==memBank.ReadBank(bank, header,2, offset))
    {
        if(header[0]==GSId::GSTrackId)
        {
            if(sizeof(track)==memBank.ReadBank(bank, (uint8_t*)(&track), sizeof(track), offset))
            {
                return true;
            }
        }
    }
    return false;
}

bool LoadGSCommon(MemoryBank& memBank, int bank, int offset, GSCommon& common)
{
    uint8_t header[2];
    if(2==memBank.ReadBank(bank, header,2,offset))
    {
        if(header[0]==GSId::GSCommonId)
        {
            if(sizeof(common)==memBank.ReadBank(bank, (uint8_t*)(&common), sizeof(common), offset))
            {
                return true;
            }
        }
    }
    return false;
}

bool SaveGSTrack(MemoryBank& memBank, int bank, int offset, const GSTrack& track)
{
    return sizeof(track)==memBank.WriteBank(bank, (uint8_t*)(&track), sizeof(track), offset);
}

bool SaveGSCommon(MemoryBank& memBank, int bank, int offset, const GSCommon& common)
{
    return sizeof(common)==memBank.WriteBank(bank, (uint8_t*)(&common), sizeof(common), offset);
}


MemController::MemController(MemoryBank& memoryBank)
 : m_MemoryBank(memoryBank)
 , m_Common()
 , m_CurrentPattern()
{
    Init(m_Common);
    Init(m_CurrentPattern, 32);
}

GSPattern* MemController::GetCurrentPattern()
{
    return &m_CurrentPattern;
}

void MemController::SelectBank(int bank)
{
    if(0<=bank && bank<GSMem::NumBanks)
    {
        SavePattern(m_Common.m_SelectedBank, m_Common.m_SelectedPattern, m_CurrentPattern);
        SaveGSCommon(m_MemoryBank, 0, 0, m_Common);
        m_Common.m_SelectedBank = bank;
        LoadPattern(m_Common.m_SelectedBank, m_Common.m_SelectedPattern, m_CurrentPattern);
    }
}

void MemController::SelectPattern(int pattern)
{
    if(0<=pattern && pattern<GSBank::NumPatterns)
    {
        SavePattern(m_Common.m_SelectedBank, m_Common.m_SelectedPattern, m_CurrentPattern);
        SaveGSCommon(m_MemoryBank, 0, 0, m_Common);
        m_Common.m_SelectedPattern = pattern;
        LoadPattern(m_Common.m_SelectedBank, m_Common.m_SelectedPattern, m_CurrentPattern);
    }
}

void MemController::SavePattern(int bank, int pattern, const GSPattern& gsPattern)
{
    int patternIdx =  ((bank*GSBank::NumPatterns)+pattern)*GSPattern::NumTracks;
    // half bank per track
    int selectedBank = 1 + patternIdx/2;
    for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
    {
        int offset = (idx%2)*m_MemoryBank.BankSize()/2;
        SaveGSTrack(m_MemoryBank, selectedBank+idx/2, offset, gsPattern.m_Track[idx]);
    }
}

void MemController::LoadPattern(int bank, int pattern, GSPattern& gsPattern)
{
    int patternIdx =  ((bank*GSBank::NumPatterns)+pattern)*GSPattern::NumTracks;
    // half bank per track
    int selectedBank = 1 + patternIdx/2;
    for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
    {
        int offset = (idx%2)*m_MemoryBank.BankSize()/2;
        LoadGSTrack(m_MemoryBank, selectedBank+idx/2, offset, gsPattern.m_Track[idx]);
    }
}

void MemController::LoadCommon(GSCommon& common)
{
    LoadGSCommon(m_MemoryBank, 0, 0, m_Common);
}

void MemController::SaveCommon(const GSCommon& common)
{
    SaveGSCommon(m_MemoryBank, 0, 0, m_Common);
}

void MemController::Load()
{
    // GSCommon contains current select => load from memoroy
    LoadCommon(m_Common);
    LoadPattern(m_Common.m_SelectedBank, m_Common.m_SelectedPattern, m_CurrentPattern);
}

void MemController::Save()
{
    SavePattern(m_Common.m_SelectedBank, m_Common.m_SelectedPattern, m_CurrentPattern);
    SaveCommon(m_Common);
}

