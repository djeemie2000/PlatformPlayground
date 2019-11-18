#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"

#define	LED_PIN		PB0

void setup() {
  // put your setup code here, to run once:
  pin_mode(LED_PIN, OUTPUT);
	digital_write(LED_PIN, LOW);
  //?needed? pin_mode(ADC1, OUTPUT);

  random_init(500);//TODO random from EEPROM!!!
}

void loop() {
  // put your main code here, to run repeatedly:
  digital_toggle(LED_PIN);
	sleep(200);
  digital_toggle(LED_PIN);
	sleep(200);
  
  digital_toggle(LED_PIN);
	sleep(800);

  digital_toggle(LED_PIN);
  sleep(random()>>6);//range 2^10 = 1024 msec

  digital_toggle(LED_PIN);
	sleep(800);

  int period = analog_read(ADC1);
  sleep(period);
}
