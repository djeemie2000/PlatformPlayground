#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"

class GateIn
{
public:
    GateIn() : m_PinName(0), m_History(0x00)
    {
    }

    void begin(uint8_t pinName)
    {
        m_PinName = pinName;
        attiny_pin_mode(m_PinName, ATTINY_INPUT);
        // pull up enable after pin mode input = digital write
        attiny_digital_write(m_PinName, ATTINY_HIGH);

        m_History = 0x00;
    }

    void read()
    {
        m_History = m_History << 1;

        if (attiny_digital_read(m_PinName) == ATTINY_HIGH)
        {
            m_History |= 0x01;
        }
    }

    bool Get() const { return m_History & 0x01; }
    bool IsRising() const { return (m_History & 0x03) == 0x01; /*...01*/ }
    bool IsFalling() const { return (m_History & 0x03) == 0x02; /*...10*/ }
    uint8_t GetHistory() const { return m_History; }

private:
    uint8_t m_PinName;
    uint8_t m_History;
};
