#pragma once
#include <Arduino.h>

#include "fastdigitalwrite.h"

// uses port C
template<int PinA, int PinB, int PinC>
class AnalogInBank821
{
public:
    static const int Size = 8;

    AnalogInBank821() 
    : m_ChangeThreshold(2)
    , m_UpdateIdx(0x00) 
    {}

    void Begin(int pin, int changeThreshold)
    {
        m_ChangeThreshold = changeThreshold;
        m_AnalogInPin = pin;
        pinMode(A0, OUTPUT);//TODO
        pinMode(A1, OUTPUT);
        pinMode(A2, OUTPUT);
        for(int idx = 0; idx<Size; ++idx)
        {
            m_IsChanged[idx] = false;
            m_Value[idx] = 0;
        }
        m_UpdateIdx = 0;
    }

    int Get(int idx) const
    {
        // no check on index
        return m_Value[idx];
    }

    // TODO IsChanged true during 8 updates until next update of idx
    bool IsChanged(int idx) const
    {
        // no check on index
        return m_IsChanged[idx];
    }

    void Update()
    {
        ClearChanged();
        UpdateSingle();        
    }

    void UpdateAll()
    {
        ClearChanged();
        // read all
        m_UpdateIdx = 0;
        for(int idx = 0; idx<Size; ++idx)
        {
             UpdateSingle();
        }
        m_UpdateIdx = 0;
    }

private:
    void UpdateSingle()
    {
        // alternating read
        fastDigitalWritePortC<PinA>(m_UpdateIdx & 0x01);
        fastDigitalWritePortC<PinB>(m_UpdateIdx & 0x02);
        fastDigitalWritePortC<PinC>(m_UpdateIdx & 0x04);
        // digitalWrite(A0, m_UpdateIdx & 0x01);
        // digitalWrite(A1, m_UpdateIdx & 0x02);
        // digitalWrite(A2, m_UpdateIdx & 0x04);
        int idx = m_UpdateIdx&0x07;
        
        // Serial.print(m_UpdateIdx);
        // Serial.print(' ');
        // Serial.println(idx);
        
        int newValue = analogRead(m_AnalogInPin);
        int absDiff = abs(m_Value[idx] - newValue);
        if(2<absDiff)
        {
            m_Value[idx] = newValue;
            m_IsChanged[idx] = true;
        }
        else
        {
            m_IsChanged[idx] = false;
        }
        ++m_UpdateIdx;
    }


    void ClearChanged()
    {
        for(int idx = 0; idx<Size; ++idx)
        {
            m_IsChanged[idx] = false;
        }
    }

    int m_ChangeThreshold;
    int m_AnalogInPin;
    uint8_t m_UpdateIdx;
    int m_Value[Size];
    bool m_IsChanged[Size];
};

template<class BankType>
void TestAnalogInBank821(BankType& bank, int repeats)
{
    Serial.println("Test analog in bank 821...");
    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        for(int idxU = 0; idxU<8; ++ idxU)
        {
            bank.Update();
            //Serial.println(idxU);
        }

        for(int idx = 0; idx<8; ++ idx)
        {
            Serial.print(idx);
            Serial.print(' ');
            Serial.println(bank.Get(idx));
        }
        delay(500);
    }
    Serial.println("done");
}

template<int pinA, int pinB, int pinC>
void PrintChanges(AnalogInBank821<pinA, pinB, pinC>& bank)
{
    for(int idx = 0; idx<AnalogInBank821<pinA, pinB, pinC>::Size; ++idx)
    {
        if(bank.IsChanged(idx))
        {
            Serial.print("input ");
            Serial.print(idx);
            Serial.print(" is changed to ");
            Serial.println(bank.Get(idx));
        }
    }
}
