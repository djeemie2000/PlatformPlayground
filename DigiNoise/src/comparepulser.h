#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"


struct ComparePulser
{
  static const int CompareInPinA = ADC2;//PB4
  static const int CompareInPinB = ADC3;//PB3
  static const int ActionInPin = ADC1;//PB2;
    // unused:
  static const int SelectOutPinA = PB1;
  static const int SelectOutPinB = PB0;

  void setup()
  {
    m_Cntr = 0;

    attiny_pin_mode(ActionInPin, ATTINY_INPUT);
    attiny_pin_mode(SelectOutPinA, ATTINY_OUTPUT);
    attiny_pin_mode(SelectOutPinB, ATTINY_OUTPUT);
    attiny_pin_mode(CompareInPinA, ATTINY_INPUT);
    attiny_pin_mode(CompareInPinB, ATTINY_INPUT);

    // avoid noise from other pins => everything LOW upon setup
    attiny_digital_write(SelectOutPinA, ATTINY_LOW);
    attiny_digital_write(SelectOutPinB, ATTINY_LOW);

    // pulse();
    // pulse();
    // pulse();
  }

  void loop()
  {
    uint16_t valueA = attiny_analog_read(CompareInPinA);
    uint16_t valueB = attiny_analog_read(CompareInPinB);

    const uint16_t threshold = 10;
    uint16_t absDiffValue = valueA<valueB ? valueB-valueA : valueA-valueB;
    //if(valueA<valueB)
    //if(valueA+threshold<valueB && valueB+threshold<valueA)
    if(absDiffValue<threshold)
    {
      // read action from analog input:  Reset, Advance, Hold
      uint16_t action = attiny_analog_read(ActionInPin);
      if(action<256)
      {//Reset
        m_Cntr=0;
      }
      else if(action<1024-256)
      {
        // Advance
        ++m_Cntr;
      }
      //else hold

      attiny_digital_write(SelectOutPinA, m_Cntr&0x01);  
      attiny_digital_write(SelectOutPinB, (m_Cntr>>1)&0x01);    
  
      attiny_sleep(1);
      //pulse();
      //attiny_sleep(500);// only for debugging purposes
    }
 }

//  void pulse()
//  {
//     const int pulsePeriod = 200;//TODO lower!!!!
//     attiny_digital_write(ComparePulseOutPin, ATTINY_HIGH);
//     attiny_sleep(pulsePeriod);
//     attiny_digital_write(ComparePulseOutPin, ATTINY_LOW);
//     attiny_sleep(pulsePeriod);
//  }

  uint8_t m_Cntr;
};
