#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"

#include "gatein.h"
#include "integernoise32.h"

struct DualClockRandomizer
{
    // static const int ResetInPin = PB5;
    static const int CV1InPin = PB3; // ADC3; //PB3
    static const int CV2InPin = PB4; // ADC2; //PB4

    static const int ClockInPin = PB2;
    static const int ClockOutPin1 = PB1;
    static const int ClockOutPin2 = PB0;

    void setup()
    {
        m_Cntr = 0;
        m_ClockAllowed1 = 1;
        m_ClockAllowed2 = 1;

        attiny_pin_mode(ClockOutPin1, ATTINY_OUTPUT);
        attiny_pin_mode(ClockOutPin2, ATTINY_OUTPUT);
        // pin mode input needed for ADC in??
        m_ClockIn.begin(ClockInPin);

        // avoid noise from other pins => everything LOW upon setup
        attiny_digital_write(ClockOutPin1, ATTINY_LOW);
        attiny_digital_write(ClockOutPin2, ATTINY_LOW);
    }

    void loop()
    {
        uint32_t cv1 = attiny_analog_read(CV1InPin);
        uint32_t cv2 = attiny_analog_read(CV2InPin);

        // Update cntr / clock
        uint32_t period = 256;
        if (m_Cntr < period)
        {
            ++m_Cntr;
        }
        else
        {
            m_Cntr = 0;
        }

        uint32_t mask = 0x03FF;
        uint32_t noise = m_IntNoise.Rand() & mask; // scale down to 10 bits

        // remember noise from a few runs ago
        if ((m_Cntr & 0x0F) == 0)
        {
            m_Noise = noise;
        }
        else if ((m_Cntr & 0x0F) == 8)
        {
            m_Noise2 = noise;
        }

        // uint32_t noise2 = m_IntNoise.Rand() & mask; // >> 22; // scale down to 10 bits

        if (m_ClockIn.IsRising() || m_Cntr == 0)
        {
            m_ClockAllowed1 = m_Noise < cv1 ? 1 : 0;
            m_ClockAllowed2 = m_Noise2 < cv2 ? 1 : 0;
        }

        uint8_t clockInValue = m_Cntr < (period >> 1) ? 1 : 0;
        // TODO m_ClockIn.Get();

        attiny_digital_write(ClockOutPin1, clockInValue & m_ClockAllowed1);
        attiny_digital_write(ClockOutPin2, clockInValue & m_ClockAllowed2);

        attiny_usleep(100); // note : compile time constant needed for sleep
    }

    GateIn m_ClockIn;
    uint8_t m_ClockAllowed1;
    uint8_t m_ClockAllowed2;
    uint32_t m_Cntr;
    CIntegerNoise m_IntNoise;
    uint32_t m_Noise;
    uint32_t m_Noise2;
};
