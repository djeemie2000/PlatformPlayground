#pragma once

#include "GSData.h"
#include "MemoryBank.h"

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
