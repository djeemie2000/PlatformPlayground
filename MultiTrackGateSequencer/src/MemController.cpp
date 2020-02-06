#include "MemController.h"

MemController::MemController()
 : m_Memory()
 , m_CurrentPattern()
{
    Init(m_Memory, 32);//TODO load??
    Copy(m_Memory.m_Bank[m_Memory.m_Common.m_SelectedBank].m_Pattern[m_Memory.m_Common.m_SelectedPattern], m_CurrentPattern);
}

GSPattern* MemController::GetCurrentPattern()
{
    return &m_CurrentPattern;
}

void MemController::SelectBank(int bank)
{
    if(0<=bank && bank<GSMem::NumBanks)
    {
        if(!Equals(m_CurrentPattern, m_Memory.m_Bank[m_Memory.m_Common.m_SelectedBank].m_Pattern[m_Memory.m_Common.m_SelectedPattern]))
        {
            //TODO save
            Copy(m_CurrentPattern, m_Memory.m_Bank[m_Memory.m_Common.m_SelectedBank].m_Pattern[m_Memory.m_Common.m_SelectedPattern]);        
        }
        m_Memory.m_Common.m_SelectedBank = bank;
        Copy(m_Memory.m_Bank[m_Memory.m_Common.m_SelectedBank].m_Pattern[m_Memory.m_Common.m_SelectedPattern], m_CurrentPattern);
    }
}

void MemController::SelectPattern(int pattern)
{
    if(0<=pattern && pattern<GSBank::NumPatterns)
    {
        if(!Equals(m_CurrentPattern, m_Memory.m_Bank[m_Memory.m_Common.m_SelectedBank].m_Pattern[m_Memory.m_Common.m_SelectedPattern]))
        {
            //TODO save
            Copy(m_CurrentPattern, m_Memory.m_Bank[m_Memory.m_Common.m_SelectedBank].m_Pattern[m_Memory.m_Common.m_SelectedPattern]);
        }
        m_Memory.m_Common.m_SelectedPattern = pattern;
        Copy(m_Memory.m_Bank[m_Memory.m_Common.m_SelectedBank].m_Pattern[m_Memory.m_Common.m_SelectedPattern], m_CurrentPattern);
    }
}
