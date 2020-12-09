#include "ButtonState.h"

ButtonState::ButtonState(unsigned long longPressedTh)
 : m_LongPressedTh(longPressedTh)
 , m_State(0)
 , m_PrevState(0)
 , m_Millis(0) 
 , m_PrevMillis(0)
 , m_ClickedMillis(0)
{
}

void ButtonState::update(unsigned long millis, int state)
{
    m_PrevState = m_State;
    m_State = state;
    m_PrevMillis = m_Millis;
    m_Millis = millis;
    if(Clicked())
    {
        m_ClickedMillis = millis;
    }
}

bool ButtonState::Get() const
{
    return m_State;
}

bool ButtonState::Clicked() const
{
    return m_State && !m_PrevState;
}

bool ButtonState::Released() const
{
    return !m_State && m_PrevState;
}

bool ButtonState::Tapped() const
{
    return Released() && (m_Millis-m_ClickedMillis<m_LongPressedTh);
}

bool ButtonState::LongPressed() const
{
    return m_State && (m_PrevMillis-m_ClickedMillis<m_LongPressedTh) && (m_Millis-m_ClickedMillis>=m_LongPressedTh);
}
