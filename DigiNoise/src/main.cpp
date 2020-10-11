#include "library/include/attiny.h"
#include "library/include/attiny_digital.h"
#include "library/include/attiny_sleep.h"
#include "library/include/attiny_random.h"
#include "library/include/attiny_analog.h"
#include "library/include/attiny_timer.h"

#include "randomgate.h"
#include "pulsegate.h"
#include "masterclock.h"
#include "buttonin.h"
#include "rotaryencoder.h"

#define	LED_PIN		PB3//PB0

// RandomGate gate;
// PulseGate gate2;
// MasterClock clockOut;
ButtonIn buttonIn;
ButtonIn rotaryInA;
ButtonIn rotaryInB;
uint16_t Cntr;
RotaryEncoder rotaryEncoder;

void setup() {
 // put your setup code here, to run once:

 //attiny_random_init(500);//TODO random from EEPROM!!!
 //gate.setup();
 //gate2.setup();
 //clockOut.setup();
 rotaryInA.begin(PB0, 10);
 rotaryInB.begin(PB2, 10);
 //buttonIn.begin(PB1, 10);
 attiny_pin_mode(PB3, ATTINY_OUTPUT);
 attiny_digital_write(PB3,ATTINY_HIGH);

 attiny_pin_mode(PB4, ATTINY_OUTPUT);
 attiny_digital_write(PB4,ATTINY_HIGH);

 attiny_pin_mode(PB1, ATTINY_OUTPUT);
 attiny_timer_mode(FAST_PWM);
 attiny_timer_prescale(1);
 Cntr = 0;
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
  //TODO loop int randomPeriod = attiny_random()>>6;
  attiny_sleep(500);//range 2^10 = 1024 msec
  //TODO variable cntr + delay per millisecond??

  attiny_digital_toggle(LED_PIN);
	attiny_sleep(800);

//  int period = attiny_analog_read(ADC2);
  attiny_sleep(1000);//problem : compile time constant into sleep!!!
  //TODO variable cntr + delay per millisecond??

//  g_test.update();
}

int main()
{
  setup();

  while(true)
  {
    buttonIn.read();
    rotaryInA.read();
    rotaryInB.read();
    attiny_digital_write(PB3,rotaryInA.Get()?ATTINY_HIGH:ATTINY_LOW);
    attiny_digital_write(PB4,rotaryInB.Get()?ATTINY_HIGH:ATTINY_LOW);

    int increment = rotaryEncoder.encode(rotaryInA.Get(), rotaryInB.Get());
    Cntr += increment;
    attiny_analog_write(PB1,Cntr);
    attiny_sleep(1);
  }

  return 0;
}
