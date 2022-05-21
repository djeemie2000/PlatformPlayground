#pragma once
#include <Arduino.h>

//C0 = analog pin 0
//C1 = analog pin 1
//C2 = analog pin 2
//C3 = analog pin 3
//C4 = analog pin 4
//C5 = analog pin 5

// led / pin 13 = PB5


template<int N>
void fastDigitalWritePortB(int value)
{
  if (!value) {
		PORTB &= ~(1<<N);
	} else {
		PORTB |= (1<<N);
	}
}

template<int N>
void fastDigitalWritePortC(int value)
{
  if (!value) {
		PORTC &= ~(1<<N);
	} else {
		PORTC |= (1<<N);
	}
}


template<int N>
void fastDigitalWritePortD(int value)
{
  if (!value) {
		PORTD &= ~(1<<N);
	} else {
		PORTD |= (1<<N);
	}
}

template<int N>
int fastDigitalReadPortC()
{
	// output 1 or 0
	return (PINC & (1<<N)) ? 1 : 0;
}
