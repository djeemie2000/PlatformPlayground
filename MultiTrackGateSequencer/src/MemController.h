#pragma once

#include "GSData.h"

class MemController
{
public:
    MemController();

    GSPattern* GetCurrentPattern();

    void SelectBank(int bank);
    void SelectPattern(int pattern);

private:
    GSMem m_Memory;
    GSPattern m_CurrentPattern;
};
