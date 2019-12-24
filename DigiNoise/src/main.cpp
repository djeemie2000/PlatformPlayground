
//#include <Arduino.h>

#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"


#define	LED_PIN		PB0

// class TestClass
// {
// public:
//   TestClass() : value(1500){}

//   int update() 
//   {
//     value  = (value + 457)%3972;
//     return value;
//   }

// private:
//   int value ;
// };

// TestClass g_test;

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

  RandomGate gate;
  PulseGate gate2;


void setup() {
  // put your setup code here, to run once:
  attiny_pin_mode(LED_PIN, ATTINY_OUTPUT);
	attiny_digital_write(LED_PIN, ATTINY_LOW);
  //?needed? pin_mode(ADC1, OUTPUT);

  attiny_random_init(500);//TODO random from EEPROM!!!

 //gate.setup();
 //gate2.setup();
}

void loop() {
  //gate.loop();  
  //gate2.loop();

  // put your main code here, to run repeatedly:
  attiny_digital_toggle(LED_PIN);
	attiny_sleep(200);
  attiny_digital_toggle(LED_PIN);
	attiny_sleep(200);
  
  attiny_digital_toggle(LED_PIN);
	attiny_sleep(800);

  attiny_digital_toggle(LED_PIN);
  int randomPeriod = attiny_random()>>6;
  attiny_sleep(500);//range 2^10 = 1024 msec
  //TODO variable cntr + delay per millisecond??

  attiny_digital_toggle(LED_PIN);
	attiny_sleep(800);

  int period = attiny_analog_read(ADC1);
  attiny_sleep(1000);//problem : compile time constant into sleep!!!
  //TODO variable cntr + delay per millisecond??

//  g_test.update();
}

int main()
{
  setup();

  while(true)
  {
    loop();
  }

  return 0;
}
