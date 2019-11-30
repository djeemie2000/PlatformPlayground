/**
 * Copyright (c) 2015 by Lukasz Marcin Podkalicki <lukasz@podkalicki.com>
 */

#ifndef	_ATTINY_TIMER_H_
#define	_ATTINY_TIMER_H_

#include <avr/interrupt.h>
#include "attiny.h"

typedef enum {
	NORMAL = 0,
	PWM = 1,
	CTC = 2,
	FAST_PWM = 3,
	PWMX = 4,
	FAST_PWMX = 5
} wgm_t;

void attiny_timer_prescale(uint16_t value);
void attiny_timer_mode(wgm_t mode);

// TIMER0 OVF
#define	attiny_timer_ovf_handler()	ISR(TIM0_OVF_vect)
#define	attiny_timer_ovf_enable() 	TIMSK0 |= 1 << TOIE0
#define	attiny_timer_ovf_disable()	TIMSK0 &= ~(1 << TOIE0)

// TIMER0 COMPA
#define attiny_timer_compa_handler()	ISR(TIM0_COMPA_vect)
#define	attiny_timer_compa_enable()	TIMSK0 |= 1 << OCIE0A
#define	attiny_timer_compa_disable()	TIMSK0 &= ~(1 << OCIE0A)

// TIMER0 COMPB
#define attiny_timer_compb_handler()	ISR(TIM0_COMPB_vect)
#define attiny_timer_compb_enable()	TIMSK0 |= 1 << OCIE0B
#define attiny_timer_compb_disable()	TIMSK0 &= ~(1 << OCIE0B)

#endif	/* !_ATTINY_TIMER_H_ */
