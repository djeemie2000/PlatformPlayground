#pragma once

#include <Arduino.h>

#include "debouncer.h"

class ButtonIn
{
public:
    ButtonIn() :  m_PinName(0),m_History(0x00)
    {}

    void begin(uint8_t pinName, uint8_t debounce = 5)
    {
        m_PinName = pinName;
        pinMode(m_PinName, INPUT_PULLUP);

        m_History = 0x00;
        m_Debouncer.begin(debounce);
    }

    void read()
    {
        m_History = m_History << 1;

        //debounce
        bool value = (digitalRead(m_PinName)==HIGH);
        m_Debouncer.debounce(value);
        if(m_Debouncer.Get())
        {
            m_History |= 0x01;
        }
    }

    bool Get() const{return m_History&0x01;}
    bool IsRising() const {return (m_History & 0x03) == 0x01;/*...01*/ }
    bool IsFalling() const { return (m_History & 0x03) == 0x02;/*...10*/}
    uint8_t GetHistory() const{return m_History;}
private:
    uint8_t m_PinName;
    uint8_t m_History;
    Debouncer m_Debouncer;
};
