#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"

#include "gatein.h"

struct MasterClock
{
  static const int SpeedInPin = ADC2; //PB4
  static const int ResetInPin = PB3;
  static const int ClockOutPin1 = PB2;
  static const int ClockOutPin2 = PB1;
  static const int ClockOutPin3 = PB0;

  void setup()
  {
    m_Cntr = 0;
    m_ClockOutBits = 0xFF;
    attiny_pin_mode(ClockOutPin1, ATTINY_OUTPUT);
    attiny_pin_mode(ClockOutPin2, ATTINY_OUTPUT);
    attiny_pin_mode(ClockOutPin3, ATTINY_OUTPUT);
    // pin mode input needed for ADC in??
    m_Reset.begin(ResetInPin);
  }

  void loop()
  {
    uint32_t period = attiny_analog_read(SpeedInPin);

    // period = period * period >> 10u; //rescale
    //16 to 128 msec period (1<<4 to 1<<7)
    // band 16-32
    // band 32-64
    // band 64-128
    // alt: 30 to 240 BPM
    // 30-60 60-120 120-240
    // alt 60 to 120 BPM 32 to 64 to 128 msec

    // TODO mapping
    if (period < 512)
    {
      // rescale [0,512] to [32,64]
      period = 32 + (period >> 5);
    }
    else
    {
      period -= 512;
      // rescale [0,512] to [64,128]
      period = 64 + (period >> 4);
    }

    // period = period >> 3;
    // if (period < 16)
    // {
    //   period = 16;
    // }

    // reset
    if (m_Reset.IsRising())
    {
      m_ClockOutBits = 0x00; // => next -- => 0xFF;
    }
    //debug !!!!!!!!
    //period = 32;

    // Update cntr / clock
    if (m_Cntr < period)
    {
      ++m_Cntr;
    }
    else
    {
      m_Cntr = 0;
      --m_ClockOutBits; //increment clock out
    }

    // clock out bits to outputs => clock divider /1 /2 /4 /8 ...
    attiny_digital_write(ClockOutPin1, m_ClockOutBits & 0x01);
    attiny_digital_write(ClockOutPin2, (m_ClockOutBits >> 1) & 0x01);
    attiny_digital_write(ClockOutPin3, (m_ClockOutBits >> 2) & 0x01);

    // if(550<=period)
    // {
    //   attiny_digital_write(ClockOutPin3, ATTINY_HIGH);
    // }
    // else
    // {
    //   attiny_digital_write(ClockOutPin3, ATTINY_LOW);
    // }

    attiny_usleep(100); //note : compile time constant needed for sleep
  }

  GateIn m_Reset;
  uint8_t m_ClockOutBits;
  uint32_t m_Cntr;
};
