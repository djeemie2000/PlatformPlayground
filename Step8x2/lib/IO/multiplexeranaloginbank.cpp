#include "multiplexeranaloginbank.h"

MultiplexerAnaloginIn::MultiplexerAnaloginIn()
    : m_SelectAPin(-1)
    , m_SelectBPin(-1)
    , m_SelectCPin(-1)
    , m_AInPin1(-1)
    , m_AInPin2(-1) 
    , m_CurrentIdx(0)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        m_Values0[idx] = 0;
        m_Values1[idx] = 0;
    }
}

void MultiplexerAnaloginIn::begin(int selectAPin, int selectBPin, int selectCPin, int aInPin1, int aInPin2)
{
    m_SelectAPin = selectAPin;
    m_SelectBPin = selectBPin;
    m_SelectCPin = selectCPin;
    m_AInPin1 = aInPin1;
    m_AInPin2 = aInPin2;
    m_CurrentIdx = 0;

    pinMode(m_SelectAPin, OUTPUT); 
    pinMode(m_SelectBPin, OUTPUT); 
    pinMode(m_SelectCPin, OUTPUT); 
    pinMode(m_AInPin1, INPUT_PULLUP);//PULLUP?
    pinMode(m_AInPin2, INPUT_PULLUP);//PULLUP?
}

int MultiplexerAnaloginIn::Get(int idxBank, int idx) const
{   
    int retVal = -1;
    if(0<=idx && idx<Size)
    {
        if(0 == idxBank)
        {
            retVal = m_Values0[idx];
        }
        else if(1 == idxBank)
        {
            retVal = m_Values1[idx];
        }
    }

    // Serial.print("G ");
    // Serial.print(idxBank);
    // Serial.print(' ');
    // Serial.print(idx);
    // Serial.print(':');
    // Serial.println(retVal);

    return retVal;
}

void MultiplexerAnaloginIn::updateOne()
{
    digitalWrite(m_SelectAPin, m_CurrentIdx & 0x01);
    digitalWrite(m_SelectBPin, (m_CurrentIdx >> 1) & 0x01);
    digitalWrite(m_SelectCPin, (m_CurrentIdx >> 2) & 0x01);

    m_Values0[m_CurrentIdx] = analogRead(m_AInPin1);
    m_Values1[m_CurrentIdx] = analogRead(m_AInPin2);

    // Serial.print(tmp1);
    // Serial.print(' ');
    // Serial.print(m_Values0[m_CurrentIdx]);
    // Serial.print(' ');
    // Serial.print(tmp2);
    // Serial.print(' ');
    // Serial.println(m_Values1[m_CurrentIdx]);

    // in case size = 8, can keep on counting up, no need to reset to zero!
    ++m_CurrentIdx;
    if(Size<=m_CurrentIdx)
    {
        m_CurrentIdx = 0;
    }

    //delay(200);
}

void MultiplexerAnaloginIn::updateAll()
{
    for(int repeat = 0; repeat<Size; ++repeat)
    {
        updateOne();
    }
}


void printAnalogInBank(MultiplexerAnaloginIn &analogInBank)
{
    analogInBank.updateAll();

    Serial.println("AIn:");
    for(int idxBank = 0; idxBank<MultiplexerAnaloginIn::Banks; ++idxBank)
    {
        Serial.print(idxBank);
        // Serial.print(" ");
        // Serial.print(bank.m_AInPin1);
        // Serial.print(" ");
        // Serial.print(bank.m_AInPin2);
        Serial.print( " : ");
        for(int idx = 0; idx<MultiplexerAnaloginIn::Size; ++idx)
        {
            Serial.print(analogInBank.Get(idxBank, idx));
            Serial.print(" ");
        }
        Serial.println();
    }
//    Serial.println();
}

void testAnalogInBank(MultiplexerAnaloginIn &bank, int repeats)
{
    for(int repeat=0; repeat<repeats || repeats<0; ++repeat)
    {
        printAnalogInBank(bank);
        delay(500);
    }
}


