#include "MemController.h"

MemController::MemController()
 : m_Memory()
 , m_CurrentPattern()
 , m_SelectedBank(0)
 , m_SelectedPattern(0)
{
    Copy(m_Memory.m_Bank[m_SelectedBank].m_Pattern[m_SelectedPattern], m_CurrentPattern);
}

GSPattern* MemController::GetCurrentPattern()
{
    return &m_CurrentPattern;
}

void MemController::SelectBank(int bank)
{
    if(0<=bank && bank<GSMem::NumBanks)
    {
        Copy(m_CurrentPattern, m_Memory.m_Bank[m_SelectedBank].m_Pattern[m_SelectedPattern]);
        m_SelectedBank = bank;
        Copy(m_Memory.m_Bank[m_SelectedBank].m_Pattern[m_SelectedPattern], m_CurrentPattern);
    }
}

void MemController::SelectPattern(int pattern)
{
    if(0<=pattern && pattern<GSBank::NumPatterns)
    {
        Copy(m_CurrentPattern, m_Memory.m_Bank[m_SelectedBank].m_Pattern[m_SelectedPattern]);
        m_SelectedPattern = pattern;
        Copy(m_Memory.m_Bank[m_SelectedBank].m_Pattern[m_SelectedPattern], m_CurrentPattern);
    }
}
