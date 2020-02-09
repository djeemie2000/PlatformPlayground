#include "MemController.h"


bool LoadGSTrack(MemoryBank& memBank, int bank, GSTrack& track)
{
    uint8_t header[2];
    if(2==memBank.ReadBank(bank, header,2))
    {
        if(header[0]==GSId::GSTrackId)
        {
            if(sizeof(track)==memBank.ReadBank(bank, (uint8_t*)(&track), sizeof(track)))
            {
                return true;
            }
        }
    }
    return false;
}

bool LoadGSCommon(MemoryBank& memBank, int bank, GSCommon& common)
{
    uint8_t header[2];
    if(2==memBank.ReadBank(bank, header,2))
    {
        if(header[0]==GSId::GSCommonId)
        {
            if(sizeof(common)==memBank.ReadBank(bank, (uint8_t*)(&common), sizeof(common)))
            {
                return true;
            }
        }
    }
    return false;
}

bool SaveGSTrack(MemoryBank& memBank, int bank, const GSTrack& track)
{
    return sizeof(track)==memBank.WriteBank(bank, (uint8_t*)(&track), sizeof(track));
}

bool SaveGSCommon(MemoryBank& memBank, int bank, const GSCommon& common)
{
    return sizeof(common)==memBank.WriteBank(bank, (uint8_t*)(&common), sizeof(common));
}


MemController::MemController(MemoryBank& memoryBank)
 : m_MemoryBank(memoryBank)
 , m_Memory()
 , m_CurrentPattern()
{
    Init(m_Memory, 32);
    //TODO load all!
    Copy(GetSelectedPatternInternal(), m_CurrentPattern);
}

GSPattern* MemController::GetCurrentPattern()
{
    return &m_CurrentPattern;
}

GSPattern& MemController::GetSelectedPatternInternal()
{
    return GetSelectedPatternInternal();   
}


void MemController::SelectBank(int bank)
{
    if(0<=bank && bank<GSMem::NumBanks)
    {
        if(!Equals(m_CurrentPattern, GetSelectedPatternInternal()))
        {
            //TODO save SaveSelectedPatternInternal();
            Copy(m_CurrentPattern, GetSelectedPatternInternal());        
        }
        m_Memory.m_Common.m_SelectedBank = bank;
        Copy(GetSelectedPatternInternal(), m_CurrentPattern);
    }
}

void MemController::SelectPattern(int pattern)
{
    if(0<=pattern && pattern<GSBank::NumPatterns)
    {
        if(!Equals(m_CurrentPattern, GetSelectedPatternInternal()))
        {
            //TODO save SaveSelectedPatternInternal();
            Copy(m_CurrentPattern, GetSelectedPatternInternal());
        }
        m_Memory.m_Common.m_SelectedPattern = pattern;
        Copy(GetSelectedPatternInternal(), m_CurrentPattern);
    }
}

void MemController::SaveSelectedPatternInternal()
{
    // GSCommon contains current select => save to bank 0
    SaveGSCommon(m_MemoryBank, 0, m_Memory.m_Common);

    //save pattern (tracks)
    int bank = 1 + ((m_Memory.m_Common.m_SelectedBank*GSBank::NumPatterns)+m_Memory.m_Common.m_SelectedPattern)*GSPattern::NumTracks;
    GSPattern& pattern = GetSelectedPatternInternal();
    for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
    {
        SaveGSTrack(m_MemoryBank, bank+idx, pattern.m_Track[idx]);
    }
}
