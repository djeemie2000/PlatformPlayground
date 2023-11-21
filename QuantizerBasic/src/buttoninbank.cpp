#include "buttoninbank.h"

ButtonInBank::ButtonInBank() : m_Debounce(4), m_Millis(0)
{
    for (int idx = 0; idx < ButtonInBank::Size; ++idx)
    {
        m_History[idx] = 0x00;
    }
}

void ButtonInBank::Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3)
{
    m_InputPin[0] = pin0;
    m_InputPin[1] = pin1;
    m_InputPin[2] = pin2;
    m_InputPin[3] = pin3;

    for (int idx = 0; idx < ButtonInBank::Size; ++idx)
    {
        m_History[idx] = 0x00;
        pinMode(m_InputPin[idx], INPUT_PULLUP);
    }
}

void ButtonInBank::SetDebounce(int debounce)
{
    m_Debounce = debounce;
}

bool ButtonInBank::Get(int idx) const
{
    // xxxxxxx1
    return 0x01 == (m_History[idx] & 0x01);
}

bool ButtonInBank::IsClicked(int idx) const
{
    // xxxxxx01
    return m_Sampled && (0x01 == (m_History[idx] & 0x03));
}

bool ButtonInBank::IsReleased(int idx) const
{
    // xxxxxx10
    return m_Sampled && (0x02 == (m_History[idx] & 0x03));
}

void ButtonInBank::Update(unsigned long millis)
{
    unsigned long elapsed = millis - m_Millis;

    m_Sampled = m_Debounce < elapsed;
    if (m_Sampled)
    {
        m_Millis = millis;

        for (int idx = 0; idx < ButtonInBank::Size; ++idx)
        {
            int temp = 1-digitalRead(m_InputPin[idx]);// button with PULLUP => invert
            m_History[idx] = (m_History[idx] << 1) | temp;
        }
    }
}

void TestButtonIn(ButtonInBank &buttonIn, int repeats)
{
    Serial.print("Testing button in bank...");
    int eventCount = 0;
    for (int repeat = 0; repeat < repeats; ++repeat)
    {
        for (int idx = 0; idx < 1000; ++idx)
        {
            buttonIn.Update(millis());

            for (int btn = 0; btn < ButtonInBank::Size; ++btn)
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
    Serial.println("done");
}

void TestButtonInGet(ButtonInBank &buttonIn, int repeats)
{
    Serial.println("Testing button in bank get...");
    for (int repeat = 0; repeat < repeats; ++repeat)
    {
        for (int idx = 0; idx < 500; ++idx)
        {
            buttonIn.Update(millis());
            delay(1);
        }
        for (int btn = 0; btn < ButtonInBank::Size; ++btn)
        {
            Serial.print(buttonIn.Get(btn) ? 1 : 0);
        }
        Serial.println();
    }
    Serial.println("done");
}
