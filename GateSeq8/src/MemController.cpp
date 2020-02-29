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
 , m_Memory()
 , m_CurrentPattern()
{
    Init(m_Memory, 32);
    
    //LoadAllPatterns();
    //Copy(GetSelectedPatternInternal(), m_CurrentPattern);
}

GSPattern* MemController::GetCurrentPattern()
{
    return &m_CurrentPattern;
}

GSPattern& MemController::GetSelectedPatternInternal()
{
    int bank = m_Memory.m_Common.m_SelectedBank;
    int pattern = m_Memory.m_Common.m_SelectedPattern;
    return m_Memory.m_Bank[bank].m_Pattern[pattern];   
}

void MemController::SelectBank(int bank)
{
    if(0<=bank && bank<GSMem::NumBanks)
    {
        //if(!Equals(m_CurrentPattern, GetSelectedPatternInternal()))
        {
            //TODO save : 
            Copy(m_CurrentPattern, GetSelectedPatternInternal());        
            SaveSelectedPatternInternal();
        }
        m_Memory.m_Common.m_SelectedBank = bank;
        Copy(GetSelectedPatternInternal(), m_CurrentPattern);
    }
}

void MemController::SelectPattern(int pattern)
{
    if(0<=pattern && pattern<GSBank::NumPatterns)
    {
//        if(!Equals(m_CurrentPattern, GetSelectedPatternInternal()))
        {
            //TODO save : 
            Copy(m_CurrentPattern, GetSelectedPatternInternal());
            SaveSelectedPatternInternal();
        }
        m_Memory.m_Common.m_SelectedPattern = pattern;
        Copy(GetSelectedPatternInternal(), m_CurrentPattern);
    }
}

void MemController::SaveSelectedPatternInternal()
{
    // GSCommon contains current select => save to bank 0
    SaveGSCommon(m_MemoryBank, 0, 0, m_Memory.m_Common);

    //save pattern (tracks)
    int selectedBank = m_Memory.m_Common.m_SelectedBank;
    int selectedPattern = m_Memory.m_Common.m_SelectedPattern;
    SavePattern(selectedBank, selectedPattern);
    // int selectedTrackIdx =  ((selectedBank*GSBank::NumPatterns)+selectedPattern)*GSPattern::NumTracks;
    // // half bank per track
    // int bank = 1 + selectedTrackIdx/2;
    // GSPattern& pattern = GetSelectedPatternInternal();
    // for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
    // {
    //     int offset = (idx%2)*m_MemoryBank.BankSize()/2;
    //     SaveGSTrack(m_MemoryBank, bank+idx/2, offset, pattern.m_Track[idx]);
    // }
}

void MemController::SavePattern(int bank, int pattern)
{
    int patternIdx =  ((bank*GSBank::NumPatterns)+pattern)*GSPattern::NumTracks;
    // half bank per track
    int selectedBank = 1 + patternIdx/2;
    GSPattern& gsPattern = m_Memory.m_Bank[bank].m_Pattern[pattern];
    for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
    {
        int offset = (idx%2)*m_MemoryBank.BankSize()/2;
        SaveGSTrack(m_MemoryBank, selectedBank+idx/2, offset, gsPattern.m_Track[idx]);
    }
}

void MemController::LoadPattern(int bank, int pattern)
{
    int patternIdx =  ((bank*GSBank::NumPatterns)+pattern)*GSPattern::NumTracks;
    // half bank per track
    int selectedBank = 1 + patternIdx/2;
    GSPattern& gsPattern = m_Memory.m_Bank[bank].m_Pattern[pattern];
    for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
    {
        int offset = (idx%2)*m_MemoryBank.BankSize()/2;
        LoadGSTrack(m_MemoryBank, selectedBank+idx/2, offset, gsPattern.m_Track[idx]);
    }
}

void MemController::LoadCommon()
{
    // GSCommon contains current select => load from bank 0
    LoadGSCommon(m_MemoryBank, 0, 0, m_Memory.m_Common);

    // assumes all patterns have been loaded!
    Copy(GetSelectedPatternInternal(), m_CurrentPattern);
}

void MemController::LoadAllPatterns()
{
    // GSCommon contains current select => load from bank 0
    LoadGSCommon(m_MemoryBank, 0, 0, m_Memory.m_Common);

    for(int selectedBank = 0; selectedBank<GSMem::NumBanks; ++selectedBank)
    {
        for(int selectedPattern = 0; selectedPattern<GSBank::NumPatterns; ++selectedPattern)
        {
            LoadPattern(selectedBank, selectedPattern);
            // int selectedTrackIdx =  ((selectedBank*GSBank::NumPatterns)+selectedPattern)*GSPattern::NumTracks;
            // // half bank per track
            // int bank = 1 + selectedTrackIdx/2;
            // GSPattern& pattern = m_Memory.m_Bank[selectedBank].m_Pattern[selectedPattern];
            // for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
            // {
            //     int offset = (idx%2)*m_MemoryBank.BankSize()/2;
            //     LoadGSTrack(m_MemoryBank, bank+idx/2, offset, pattern.m_Track[idx]);
            // }
        }
    }
}

