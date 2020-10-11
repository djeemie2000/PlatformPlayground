#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"

#include "buttonin.h"
#include "rotaryencoder.h"
#include "rotaryvalue.h"

struct MasterClock2
{
  static const int ClockOutPin1 = PB1;
  //static const int ClockOutPin2 = PB4;
  static const int modeOutPin = PB4;

  void setup()
  {
    m_Cntr = 0;
    m_ClockOutBits = 0xFF;
    m_Mode = 0;

    rotaryInA.begin(PB0, 10);
    rotaryInB.begin(PB2, 10);
    buttonIn.begin(PB3, 10);

    m_Period.begin(32,8096, 4);
    m_Period.reset(1000);
    m_Divide.begin(0,40,1);
    m_Divide.reset(4);

    attiny_pin_mode(ClockOutPin1, ATTINY_OUTPUT);
    attiny_pin_mode(modeOutPin, ATTINY_OUTPUT);
  }

  void loop()
  {
    rotaryInA.read();
    rotaryInB.read();
    buttonIn.read();

    int increment = rotaryEncoder.encode(rotaryInA.Get(), rotaryInB.Get());
    if(m_Mode == 0)
    {
        m_Period.update(increment);
    }
    else if(m_Mode == 1)
    {
        m_Divide.update(increment);
    }
    if(buttonIn.IsFalling())
    {
       //toggle mode
        m_Mode = 1-m_Mode;
    }

    int period = m_Period.get(); 
    period = period >> (m_Divide.get()>>2);//4x oversampling
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
    //attiny_digital_write(ClockOutPin2, (m_ClockOutBits>>1)&0x01);    
    
    attiny_digital_write(modeOutPin, m_Mode);//0 or 1  

    attiny_sleep(1);//note : compile time constant needed for sleep
  }

    uint8_t m_ClockOutBits;
    int m_Cntr;
    int m_Mode;

    ButtonIn buttonIn;
    ButtonIn rotaryInA;
    ButtonIn rotaryInB;
    RotaryEncoder rotaryEncoder;

    RotaryValue m_Period;
    RotaryValue m_Divide;
};
