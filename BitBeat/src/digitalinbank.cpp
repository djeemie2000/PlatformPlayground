#include "digitalinbank.h"

DigitalInBank::DigitalInBank()
{
    for (int idx = 0; idx < DigitalInBank::Size; ++idx)
    {
        m_History[idx] = 0x00;
    }
}

void DigitalInBank::Begin(uint8_t pin0, uint8_t pin1)
{
    m_InputPin[0] = pin0;
    m_InputPin[1] = pin1;

    for (int idx = 0; idx < DigitalInBank::Size; ++idx)
    {
        m_History[idx] = 0x00;
        pinMode(m_InputPin[idx], INPUT_PULLUP);//PULLUP needed?
    }
}

bool DigitalInBank::Get(int idx) const
{
    // xxxxxxx1
    return 0x01 == (m_History[idx] & 0x01);
}

bool DigitalInBank::IsClicked(int idx) const
{
    // xxxxxx01
    return 0x01 == (m_History[idx] & 0x03);
}

bool DigitalInBank::IsReleased(int idx) const
{
    // xxxxxx10
    return 0x02 == (m_History[idx] & 0x03);
}

void DigitalInBank::Update()
{
    for (int idx = 0; idx < DigitalInBank::Size; ++idx)
    {
        int temp = 1 - digitalRead(m_InputPin[idx]); // PULLUP => invert
        m_History[idx] = (m_History[idx] << 1) | temp;
    }
}

void TestButtonIn(DigitalInBank &buttonIn, int repeats)
{
    Serial.println("Testing digital in bank...");
    int eventCount = 0;
    for (int repeat = 0; repeat < repeats; ++repeat)
    {
        for (int idx = 0; idx < 1000; ++idx)
        {
            buttonIn.Update();

            for (int btn = 0; btn < DigitalInBank::Size; ++btn)
            {
                if (buttonIn.IsClicked(btn))
                {
                    Serial.print(++eventCount);
                    Serial.print(" Button ");
                    Serial.print(btn);
                    Serial.println(" is clicked");
                }

                if (buttonIn.IsReleased(btn))
                {
                    Serial.print(++eventCount);
                    Serial.print(" Button ");
                    Serial.print(btn);
                    Serial.println(" is released");
                }
            }

            delay(1);
        }
    }
}

void TestButtonInGet(DigitalInBank &buttonIn, int repeats)
{
    Serial.println("Testing digital in bank get...");
    for (int repeat = 0; repeat < repeats; ++repeat)
    {
        for (int idx = 0; idx < 500; ++idx)
        {
            buttonIn.Update();
            delay(1);
        }
        for (int btn = 0; btn < DigitalInBank::Size; ++btn)
        {
            Serial.print(buttonIn.Get(btn) ? 1 : 0);
        }
        Serial.println();
    }
}
