
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

struct RandomGate
{
  static const int DurationPin = ADC3;
  static const int RandomDepthPin = ADC2;
  static const int OutPin1 = PB1;

  int m_Duration1;
  int m_Cntr1;

  void setup()
  {
    int randomSeed = analog_read(DurationPin) + analog_read(RandomDepthPin);
    attiny_random_init(randomSeed);

    m_Cntr1 = 0;
    m_Duration1 = analog_read(DurationPin);
  }

  void loop()
  {
    // [0, 2^10] = [0,1024]

    // [-1024, +1024]
    int32_t randomValue = (attiny_random()>>5)-(1<<10);

    int32_t duration = analog_read(DurationPin);
    int32_t depth = analog_read(RandomDepthPin);

    // duration * ( 1 + depth * (2*randomValue-1) )
    int32_t value = duration * ((1l<<20l)+depth*randomValue)>>20l;
    // min value
    value += 10;

    ++m_Cntr1;
    if(m_Duration1<=m_Cntr1)
    {
      digital_toggle(OutPin1);
      m_Duration1 = duration;//TODO value
      m_Cntr1 = 0;
    }

    // sleep determines scaling of duration
    attiny_sleep(1);//?
  }
};

  RandomGate gate;


void setup() {
  // put your setup code here, to run once:
  pin_mode(LED_PIN, ATTINY_OUTPUT);
	digital_write(LED_PIN, ATTINY_LOW);
  //?needed? pin_mode(ADC1, OUTPUT);

  attiny_random_init(500);//TODO random from EEPROM!!!

 // gate.setup();
}

void loop() {
  //gate.loop();  

  // put your main code here, to run repeatedly:
  digital_toggle(LED_PIN);
	attiny_sleep(200);
  digital_toggle(LED_PIN);
	attiny_sleep(200);
  
  digital_toggle(LED_PIN);
	attiny_sleep(800);

  digital_toggle(LED_PIN);
  int randomPeriod = attiny_random()>>6;
  attiny_sleep(500);//range 2^10 = 1024 msec
  //TODO variable cntr + delay per millisecond??

  digital_toggle(LED_PIN);
	attiny_sleep(800);

  int period = analog_read(ADC1);
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
