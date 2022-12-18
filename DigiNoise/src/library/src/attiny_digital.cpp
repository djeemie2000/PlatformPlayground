/**
 * Copyright (c) 2015 by Lukasz Marcin Podkalicki <lukasz@podkalicki.com>
 */

#include <avr/interrupt.h>
#include "../include/attiny_io.h"
#include "../include/attiny_utils.h"
#include "../include/attiny_digital.h"

uint8_t
attiny_digital_read(uint8_t pin)
{

	if ((PINB & (1 << pin)) > 0)
		return ATTINY_HIGH;

	return ATTINY_LOW;
}

void attiny_digital_write(uint8_t pin, uint8_t value)
{
	uint8_t sreg;

	if (value != ATTINY_LOW)
	{
		sreg = SREG;
		cli();
		sbi(PORTB, pin);
		SREG = sreg;
	}
	else
	{ // value == LOW
		sreg = SREG;
		cli();
		cbi(PORTB, pin);
		SREG = sreg;
	}
}

void attiny_digital_toggle(uint8_t pin)
{

	PORTB ^= 1 << (pin);
}
