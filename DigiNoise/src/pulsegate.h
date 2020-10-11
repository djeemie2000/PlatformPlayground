#pragma once

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"

struct PulseGate
{
  static const int DurationPin = ADC3;//PB3
  static const int EnvelopePin = ADC2;//PB4
  static const int ModePin = ADC1;//PB2
  static const int GateInPin = PB1;
  static const int OutPin = PB0;

  static const int ARMode =0;//AR mode i.e. follow external gate
  static const int APRMode =1;//APR mode i.e. start pulse with (max) duration when gate in is rising
  static const int ADMode = 2; // AD mode i.e. (re)start envelope upon gate in rising, pulse off when envelope reaches maximum
  static const int ADLoopedMode = 3;// as long as gate is high, looped AD mode

  static const int MaxEnvelope = 1000;//???
  static const int MinEnvelope = 23;//???


  int m_Gate;
  int m_Cntr;
  int m_PulseOut;

  void setup()
  {
    m_Cntr = 0;
    m_PulseOut = ATTINY_LOW;
    m_Gate = ATTINY_LOW;

    //TODO
    attiny_pin_mode(GateInPin, ATTINY_INPUT);
    attiny_pin_mode(OutPin, ATTINY_OUTPUT);
	  attiny_digital_write(OutPin, ATTINY_LOW);
  }


  void loop()
  {
    //TODO
    int durationIn = attiny_analog_read(DurationPin);
    int envelopeIn = attiny_analog_read(EnvelopePin);
    int modeIn = attiny_analog_read(ModePin);
    int gateIn = attiny_digital_read(GateInPin);

    // update gate in state
    bool gateRising = !m_Gate && gateIn;
    bool gateFalling = m_Gate && !gateIn;
    m_Gate = gateIn;
    
    //TODO mode from analog in
    int mode = APRMode; 
    if(1010<modeIn)
    {
      mode = ARMode;
    }
    else if(512<modeIn)
    {
      mode= APRMode;
    }
    else if(modeIn<10)
    {
      mode = ADLoopedMode;
    }
    else
    {
      mode = ADMode;
    }
    

    if(mode==ARMode)
    {
      //AR mode i.e. follow external gate
      m_PulseOut = gateIn;
    }
    else if(mode==APRMode)
    {
      //APR mode i.e. start pulse with (max) duration when gate in is rising
      // rising gate => reset cntr, output on
      // cntr>duration or falling gate => output off

      // min 10 msec gate out??
      // max 2046+10 msec gate out?
      int duration = 10 + 2*durationIn;
      if(gateRising)
      {
        m_Cntr = 0;
        m_PulseOut = ATTINY_HIGH;
      }
      else if(gateFalling)
      {
        m_PulseOut = ATTINY_LOW;
      }
      else if(m_PulseOut && duration<m_Cntr)
      {
        m_PulseOut = ATTINY_LOW;
      }
      ++m_Cntr;
    }
    else if(mode==ADMode)
    {
       // AD mode i.e. (re)start envelope upon gate in rising, pulse off when envelope reaches maximum
      if(gateRising)
      {
        m_Cntr = 0;
        m_PulseOut = ATTINY_HIGH;
      }
      else if(m_PulseOut && MaxEnvelope<=envelopeIn)
      {
        m_PulseOut = ATTINY_LOW;
      }
      ++m_Cntr;
    }
    else if(mode==ADLoopedMode)
    {
      if(gateIn==ATTINY_LOW)
      {
        m_PulseOut = ATTINY_LOW;
      }
      else if(m_PulseOut && MaxEnvelope<=envelopeIn)
      {
        m_PulseOut = ATTINY_LOW;
      } 
      else if(!m_PulseOut && envelopeIn<MinEnvelope)
      {
        m_PulseOut = ATTINY_HIGH;
      }
      ++m_Cntr;
    }

    // update pulse out
    attiny_digital_write(OutPin, m_PulseOut);

    attiny_sleep(1);//approx 1 msec resolution
  }
};
