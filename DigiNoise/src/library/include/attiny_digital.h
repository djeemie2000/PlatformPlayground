/**
 * Copyright (c) 2015 by Lukasz Marcin Podkalicki <lukasz@podkalicki.com>
 */

#ifndef	_ATTINY_DIGITAL_H_
#define	_ATTINY_DIGITAL_H_

#include "attiny_io.h"

uint8_t attiny_digital_read(uint8_t pin);
void attiny_digital_write(uint8_t pin, uint8_t value);
void attiny_digital_toggle(uint8_t pin);

#endif	/* !_ATTINY_DIGITAL_H_ */

