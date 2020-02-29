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

    void Load();//load common, load selected pattern into current pattern
    void Save();//save current pattern and common

    // not private for test purposes
    void LoadCommon(GSCommon& common);
    void SaveCommon(const GSCommon& common);
    void LoadPattern(int bank, int pattern, GSPattern& gsPattern);
    void SavePattern(int bank, int pattern, const GSPattern& gsPattern);

private:    
    MemoryBank& m_MemoryBank;
    GSCommon m_Common;
    GSPattern m_CurrentPattern;
};
