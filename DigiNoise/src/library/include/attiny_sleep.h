/**
 * Copyright (c) 2015 by Lukasz Marcin Podkalicki <lukasz@podkalicki.com>
 */

#ifndef	_ATTINY_SLEEP_H_
#define	_ATTINY_SLEEP_H_

#include "attiny_config.h"
#include <util/delay.h>

#define	attiny_sleep(value)	_delay_ms(value)
#define	attiny_usleep(value)	_delay_us(value)

#endif	/* !_ATTINY_SLEEP_H_ */
