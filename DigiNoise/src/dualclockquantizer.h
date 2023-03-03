#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"

#include "gatein.h"
#include "integernoise32.h"
#include "CVClockState.h"

struct DualClockQuantizer
{
    // static const int ResetInPin = PB5;
    static const int CV1InPin = PB3; // ADC3; //PB3
    static const int CV2InPin = PB4; // ADC2; //PB4

    static const int ClockInPin = PB2;
    static const int ClockOutPin1 = PB1;
    static const int ClockOutPin2 = PB0;

    void setup()
    {
        // m_Cntr = 0;

        attiny_pin_mode(ClockOutPin1, ATTINY_OUTPUT);
        attiny_pin_mode(ClockOutPin2, ATTINY_OUTPUT);

        attiny_pin_mode(ClockInPin, ATTINY_INPUT);

        // pin mode input needed for ADC in??
        attiny_pin_mode(CV1InPin, ATTINY_INPUT);
        attiny_pin_mode(CV2InPin, ATTINY_INPUT);

        m_ClockIn.begin(ClockInPin);

        // avoid noise from other pins => everything LOW upon setup
        attiny_digital_write(ClockOutPin1, ATTINY_LOW);
        attiny_digital_write(ClockOutPin2, ATTINY_LOW);
    }

    void loop()
    {
        // Update cntr / clock
        // uint32_t period = 256;
        // if (m_Cntr < period)
        // {
        //     ++m_Cntr;
        // }
        // else
        // {
        //     m_Cntr = 0;
        // }

        uint32_t cv1 = attiny_analog_read(CV1InPin);
        uint32_t cv2 = attiny_analog_read(CV2InPin);

        if (1008 < cv1)
        {
            cv1 = 1024;
        }
        else if (cv1 < 16)
        {
            cv1 = 0;
        }

        if (1008 < cv2)
        {
            cv2 = 1024;
        }
        else if (cv2 < 16)
        {
            cv2 = 0;
        }

        //        uint8_t clockInValue = m_Cntr < (period >> 1) ? 1 : 0;

        m_ClockIn.read();
        uint8_t clockInValue = m_ClockIn.Get();

        int out1 = m_State1.Tick(clockInValue, cv1);
        int out2 = m_State2.Tick(clockInValue, cv2);

        attiny_digital_write(ClockOutPin1, out1);
        attiny_digital_write(ClockOutPin2, out2);

        attiny_usleep(100); // note : compile time constant needed for sleep
    }

    GateIn m_ClockIn;
    // uint32_t m_Cntr;
    CVClockState m_State1;
    CVClockState m_State2;
};
