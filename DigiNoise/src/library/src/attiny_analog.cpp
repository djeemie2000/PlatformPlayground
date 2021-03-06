/**
 * Copyright (c) 2015 by Lukasz Marcin Podkalicki <lukasz@podkalicki.com>
 */

#include "../include/attiny_io.h"
#include "../include/attiny_utils.h"
#include "../include/attiny_analog.h"

int
attiny_analog_read(uint8_t pin)
{
	uint8_t low, high;

	switch(pin) {
	case ADC1: ADMUX = (1<<MUX0); break;
	case ADC2: ADMUX = (1<<MUX1); break;
	case ADC3: ADMUX = ((1<<MUX0)|(1<<MUX1)); break;
	case ADC0:
	default: ADMUX = 0; break;
	}

	sbi(ADCSRA, ADEN);					// Enable ADC
	sbi(ADCSRA, ADSC);					// Run single conversion
	while(bit_is_set(ADCSRA, ADSC));			// Wait conversion is done

	// Read values
	low = ADCL;
        high = ADCH;

        // combine the two bytes
        return (high << 8) | low;
}

void
attiny_analog_write(uint8_t pin, int value)
{
	attiny_pin_mode(pin, ATTINY_OUTPUT);					// Make sure the PWM output is enabled for this pin
	if (pin == DAC0) {
		sbi(TCCR0A, COM0A1); 				// connect pwm to pin on timer 0, channel A
		OCR0A = value; 					// set pwm duty
	} else if (pin == DAC1) {
		sbi(TCCR0A, COM0B1); 				// connect pwm to pin on timer 0, channel B
		OCR0B = value; 					// set pwm duty
	}
}
