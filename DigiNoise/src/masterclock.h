#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"


struct MasterClock
{
  static const int SpeedInPin = ADC2;//PB4
  static const int ClockOutPin1 = PB3;
  static const int ClockOutPin2 = PB2;
  static const int ClockOutPin3 = PB1;
  //TODO ...

  void setup()
  {
    m_Cntr = 0;
    m_ClockOutBits = 0xFF;
    attiny_pin_mode(ClockOutPin1, ATTINY_OUTPUT);
    attiny_pin_mode(ClockOutPin2, ATTINY_OUTPUT);
    attiny_pin_mode(ClockOutPin3, ATTINY_OUTPUT);
    // pin mode input needed for ADC in??
  }

  void loop()
  {
    int period = attiny_analog_read(SpeedInPin);
    if(m_Cntr<period)
    {
      ++m_Cntr;
    }
    else
    {
      m_Cntr = 0;
      --m_ClockOutBits;//increment clock out
    }
    // clock out bits to outputs => clock divider /1 /2 /4 /8 ...
    attiny_digital_write(ClockOutPin1, m_ClockOutBits&0x01);  
    attiny_digital_write(ClockOutPin2, (m_ClockOutBits>>1)&0x01);  
    attiny_digital_write(ClockOutPin3, (m_ClockOutBits>>2)&0x01);  

    // if(550<=period)
    // {
    //   attiny_digital_write(ClockOutPin3, ATTINY_HIGH);
    // }
    // else
    // {
    //   attiny_digital_write(ClockOutPin3, ATTINY_LOW);      
    // }
    

    attiny_sleep(1);//note : compile time constant needed for sleep
  }

  uint8_t m_ClockOutBits;
  int m_Cntr;
};
