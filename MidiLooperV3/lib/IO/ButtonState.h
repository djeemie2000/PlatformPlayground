#pragma once

#include <Arduino.h>

class ButtonState
{
public:
    ButtonState(unsigned long longPressedTh = 1000);

    void update(unsigned long millis, int state);

    bool Get() const;
    bool Clicked() const;
    bool Released() const;
    bool Tapped() const;
    bool LongPressed() const;

private:
    unsigned long m_LongPressedTh;
    int m_State;
    int m_PrevState;
    unsigned long m_Millis;
    unsigned long m_PrevMillis;
    unsigned long m_ClickedMillis;
};
