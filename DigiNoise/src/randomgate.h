#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"

struct RandomGate
{
  static const int DurationPin = ADC3;
  static const int RandomDepthPin = ADC2;
  static const int OutPin1 = PB1;

  int m_Duration1;
  int m_Cntr1;

  void setup()
  {
    int randomSeed = attiny_analog_read(DurationPin) + attiny_analog_read(RandomDepthPin);
    attiny_random_init(randomSeed);

    m_Cntr1 = 0;
    m_Duration1 = attiny_analog_read(DurationPin);
  }

  void loop()
  {
    // [0, 2^10] = [0,1024]

    // [-1024, +1024]
    int32_t randomValue = (attiny_random()>>5)-(1<<10);

    int32_t duration = attiny_analog_read(DurationPin);
    int32_t depth = attiny_analog_read(RandomDepthPin);

    // duration * ( 1 + depth * (2*randomValue-1) )
    int32_t value = duration * ((1l<<20l)+depth*randomValue)>>20l;
    // min value
    value += 10;

    ++m_Cntr1;
    if(m_Duration1<=m_Cntr1)
    {
      attiny_digital_toggle(OutPin1);
      m_Duration1 = duration;//TODO value
      m_Cntr1 = 0;
    }

    // sleep determines scaling of duration
    attiny_sleep(1);//?
  }
};
