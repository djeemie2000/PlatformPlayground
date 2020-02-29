#pragma once

#include "GSData.h"
#include "MemoryBank.h"

bool LoadGSTrack(MemoryBank& memBank, int bank, int offset, GSTrack& track);
bool LoadGSCommon(MemoryBank& memBank, int bank, int offset, GSCommon& common);
bool SaveGSTrack(MemoryBank& memBank, int bank, int offset, const GSTrack& track);
bool SaveGSCommon(MemoryBank& memBank, int bank, int offset, const GSCommon& common);


class MemController
{
public:
    MemController(MemoryBank& memoryBank);

    GSPattern* GetCurrentPattern();
    void SelectBank(int bank);
    void SelectPattern(int pattern);

    void LoadPattern(int bank, int pattern);
    void LoadCommon();

private:
    GSPattern& GetSelectedPatternInternal();
    void SaveSelectedPatternInternal();
    void LoadAllPatterns();
    void SavePattern(int bank, int pattern);
    
    MemoryBank& m_MemoryBank;
    GSMem m_Memory;
    GSPattern m_CurrentPattern;
};
