#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"

#define	LED_PIN		PB0

void setup() {
  // put your setup code here, to run once:
  pin_mode(LED_PIN, OUTPUT);
	digital_write(LED_PIN, LOW);

  random_init(500);//TODO random from EEPROM!!!
}

void loop() {
  // put your main code here, to run repeatedly:
  digital_toggle(LED_PIN);
	sleep(300);
  digital_toggle(LED_PIN);
	sleep(300);
  
  digital_toggle(LED_PIN);
	sleep(800);

  digital_toggle(LED_PIN);
  sleep(random()>>6);//range 2^10 = 1024 msec
}
