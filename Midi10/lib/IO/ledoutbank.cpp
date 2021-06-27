#include "ledoutbank.h"
#include <SPI.h>

LedOutBank::LedOutBank(int csPin)
    : m_CsPin(csPin), m_ValuesOn(0x00), m_ValuesOff(0x00)
{
}

void LedOutBank::begin()
{
    pinMode(m_CsPin, OUTPUT);
    m_ValuesOff = 0x00;
    m_ValuesOn = 0x00;
    // assumes;
    // SPI.begin();
    // SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
}

void LedOutBank::set(int idx, int value)
{
    if (0 <= idx && idx < Capacity)
    {
        if (value == On)
        {
            bitSet(m_ValuesOn, idx);
            bitSet(m_ValuesOff, idx);
        }
        else if (value == Off)
        {
            bitClear(m_ValuesOn, idx);
            bitClear(m_ValuesOff, idx);
        }
        else
        {
            bitSet(m_ValuesOn, idx);
            bitClear(m_ValuesOff, idx);
        }
    }
}

void LedOutBank::update(int blinkOn)
{
    digitalWrite(m_CsPin, LOW);
    SPI.transfer16(blinkOn ? m_ValuesOn : m_ValuesOff);
    digitalWrite(m_CsPin, HIGH);
}

void testLedOutBank(LedOutBank &bank, int repeats)
{
    for (int repeat = 0; repeat < repeats || repeats < 0; ++repeat)
    {
        for (int idx = 0; idx < LedOutBank::Capacity; ++idx)
        {
            for (int val = 2; 0 <= val; --val)
            {
                bank.set(idx, 1);
                bank.update(1);
                delay(200);
                bank.set(idx, 0);
                bank.update(1);
                delay(200);
            }
        }
    }
}
