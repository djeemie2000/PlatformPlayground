#include "shiftoutbank.h"
#include <SPI.h>

ShiftOutBank::ShiftOutBank()
 : m_CsPin(-1)
{
}

void ShiftOutBank::begin(int csPin)
{
    m_CsPin = csPin;
    for(int idxBank = 0; idxBank<Banks; ++idxBank)
    {
        m_Values[idxBank] = 0x00;
        m_ValuesIn[idxBank] = 0x00;
    }
    pinMode(m_CsPin, OUTPUT);
    // assumes;
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
    digitalWrite(m_CsPin, HIGH);
}

void ShiftOutBank::set(int idxBank, int idx, int value)
{
    if (0 <= idx && idx < Size && 0 <= idxBank && idxBank < Banks)
    {
        if(value)
        {
            bitSet(m_Values[idxBank], idx); 
        }
        else
        {
            bitClear(m_Values[idxBank], idx); 
        }
    }
}

int ShiftOutBank::get(int idxBank, int idx)
{
    if (0 <= idx && idx < Size && 0 <= idxBank && idxBank < Banks)
    {
        return bitRead(m_ValuesIn[idxBank], idx);
    }
    return -1;
}

void ShiftOutBank::update()
{
    digitalWrite(m_CsPin, LOW);
    for(int idxBank = 0; idxBank<Banks; ++idxBank)
    {
        m_ValuesIn[idxBank] = SPI.transfer(m_Values[idxBank]);
    }
    digitalWrite(m_CsPin, HIGH);
}

void testDigitalOutBank(ShiftOutBank &bank, int repeats)
{
    for (int repeat = 0; repeat < repeats || repeats < 0; ++repeat)
    {
        for(int idxBank = 0; idxBank<ShiftOutBank::Banks; ++idxBank)
        {
            for (int idx = 0; idx < ShiftOutBank::Size; ++idx)
            {
                bank.set(idxBank, idx, 1);
                bank.update();
                delay(200);
                
                bank.set(idxBank, idx, 0);
                bank.update();
                delay(200);
                
                bank.set(idxBank, idx, 1);
                bank.update();
            }
        }
    }
}
