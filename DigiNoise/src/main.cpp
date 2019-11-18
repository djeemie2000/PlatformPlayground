
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

void setup() {
  // put your setup code here, to run once:
  pin_mode(LED_PIN, ATTINY_OUTPUT);
	digital_write(LED_PIN, ATTINY_LOW);
  //?needed? pin_mode(ADC1, OUTPUT);

  attiny_random_init(500);//TODO random from EEPROM!!!
}

void loop() {
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
