#include "shiftoutbank.h"
#include <SPI.h>

ShiftOutBank::ShiftOutBank(int csPin)
 : m_CsPin(csPin)
{
}

void ShiftOutBank::begin()
{
    m_Values = 0;
    m_ValuesIn = 0;
    pinMode(m_CsPin, OUTPUT);
    // assumes;
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
    digitalWrite(m_CsPin, HIGH);
}

void ShiftOutBank::set(int idx, int value)
{
    if (0 <= idx && idx < Capacity)
    {
        if(value)
        {
            bitSet(m_Values,idx); 
        }
        else
        {
            bitClear(m_Values,idx); 
        }
    }
}

int ShiftOutBank::get(int idx)
{
    if (0 <= idx && idx < Capacity)
    {
        return bitRead(m_ValuesIn, idx);
    }
    return -1;
}

void ShiftOutBank::update()
{
    digitalWrite(m_CsPin, LOW);
    m_ValuesIn = SPI.transfer(m_Values);
    digitalWrite(m_CsPin, HIGH);
}

void testDigitalOutBank(ShiftOutBank &bank, int repeats)
{
    for (int repeat = 0; repeat < repeats || repeats < 0; ++repeat)
    {
        for (int idx = 0; idx < ShiftOutBank::Capacity; ++idx)
        {
            bank.set(idx, 1);
            bank.update();
            delay(200);
            
            bank.set(idx, 0);
            bank.update();
            delay(200);
            
            bank.set(idx, 1);
            bank.update();
        }
    }
}
