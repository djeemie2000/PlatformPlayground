#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"


struct DigiNoise
{
  static const int SpeedInPin = ADC2;//PB4
  static const int NoiseOutPin = PB3;
  static const int SparseNoiseOutPin = PB2;
  static const int DenseNoiseOutPin = PB1;
  static const int ClockOutPin4 = PB0;
  //TODO ...

  void setup()
  {
    attiny_pin_mode(NoiseOutPin, ATTINY_OUTPUT);
    attiny_pin_mode(SparseNoiseOutPin, ATTINY_OUTPUT);
    attiny_pin_mode(DenseNoiseOutPin, ATTINY_OUTPUT);
    attiny_pin_mode(ClockOutPin4, ATTINY_OUTPUT);
    attiny_pin_mode(SpeedInPin, ATTINY_INPUT);
    attiny_random_init(500);//seed

    // avoid noise from other pins => everything LOW upon setup
    attiny_digital_write(NoiseOutPin, ATTINY_LOW);
    attiny_digital_write(SparseNoiseOutPin, ATTINY_LOW);
    attiny_digital_write(DenseNoiseOutPin, ATTINY_LOW);
    attiny_digital_write(ClockOutPin4, ATTINY_LOW);

    // //debug : show startup
    // attiny_digital_write(SparseNoiseOutPin, ATTINY_HIGH);
    // attiny_sleep(2000);
    // attiny_digital_write(SparseNoiseOutPin, ATTINY_LOW);
  }

  void loop()
  {
    int period = attiny_analog_read(SpeedInPin);

    // debug : find low noise threshold
    // if(period<10)
    // {
    //   attiny_digital_write(SparseNoiseOutPin, ATTINY_HIGH);
    // }
    // else
    // {
    //   attiny_digital_write(SparseNoiseOutPin, ATTINY_LOW);
    // }

    // min 10 usec sleep
    const int minPeriod = 5;
    if(period<minPeriod)
    {
      period = minPeriod;
    }
    while(0<period)
    {
      --period;
      attiny_usleep(1);//note : compile time constant needed for sleep
    }

    uint16_t value = attiny_random();
    attiny_digital_write(NoiseOutPin, value&0x01);  
    // sparse noise : only HIGH when last 3 bits are all high
    attiny_digital_write(SparseNoiseOutPin, (value&0x07)==0x07);  
    // dense noise : only toggle when last 2 bits are al high/low??
    if((value&0x03)==0x00)
    {
      attiny_digital_toggle(DenseNoiseOutPin);    
    }
    //attiny_digital_toggle(SparseNoiseOutPin);//square wave out
  }

  int m_Cntr;
};
