#pragma once
#include <Arduino.h>

template<int Size>
class AnalogInBank
{
public:
    AnalogInBank() : m_UpdateIdx(0)
    {}

    void Begin()
    {
        for(int idx = 0; idx<Size; ++idx)
        {
            m_Pin[idx] = -1;
            m_Value[idx] = 0;
        }
        m_UpdateIdx = 0;
    }

    void Assign(int idx, int pin)
    {
        m_Pin[idx] = pin;
    }

    int Get(int idx) const
    {
        return m_Value[idx];
    }

    void Update()
    {
        // alternating read
        m_Value[m_UpdateIdx] = analogRead(m_Pin[m_UpdateIdx]);
        ++m_UpdateIdx;
        if(Size<=m_UpdateIdx)
        {
            m_UpdateIdx = 0;
        }
    }

    void UpdateAll()
    {
        // read all
        for(int idx = 0; idx<Size; ++idx)
        {
             m_Value[idx] = analogRead(m_Pin[idx]);
        }
        m_UpdateIdx = 0;
    }

private:
    int m_Pin[Size];
    int m_Value[Size];
    int m_UpdateIdx;
};

template<int Size>
void TestAnalogInBank(AnalogInBank<Size>& bank, int repeats)
{
    Serial.println("Test analog in bank 821...");
    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        for(int idx = 0; idx<Size; ++ idx)
        {
            bank.Update();
        }

        for(int idx = 0; idx<Size; ++ idx)
        {
            Serial.print(idx);
            Serial.print(' ');
            Serial.println(bank.Get(idx));
        }

        delay(500);
    }
    Serial.println("done");
}
