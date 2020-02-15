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

private:
    GSPattern& GetSelectedPatternInternal();
    void SaveSelectedPatternInternal();
    
    MemoryBank& m_MemoryBank;
    GSMem m_Memory;
    GSPattern m_CurrentPattern;
};
