#pragma once
#include <Arduino.h>

class MultiplexerAnaloginIn
{
public :
    static const int Size = 8;
    static const int Banks = 2;

    MultiplexerAnaloginIn();

    void begin(int selectAPin, int selectBPin, int selectCPin, int aInPin1, int aInPin2);

    int Get(int idxBank, int idx) const;

    void updateOne();
    void updateAll();

private:
    int m_SelectAPin;
    int m_SelectBPin;
    int m_SelectCPin; 
    int m_AInPin1;
    int m_AInPin2;

    uint8_t m_CurrentIdx;
    int m_Values0[Size]; 
    int m_Values1[Size]; 
};

void printAnalogInBank(MultiplexerAnaloginIn &bank);
void testAnalogInBank(MultiplexerAnaloginIn &bank, int repeats = -1);

