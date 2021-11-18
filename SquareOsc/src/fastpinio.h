#pragma once
#include <Arduino.h>

template<int N>
void FastPinOnPortD()
{
  PORTD |= (1<<N);
}

template<int N>
void FastPinOffPortD()
{
  PORTD &= ~(1<<N);
}

template<int N>
void FastPinSetPortD(int value)
{
  if(value)
  {
    FastPinOnPortD<N>();
  }
  else
  {
    FastPinOffPortD<N>();
  }
}

template<int N>
int FastPinGetPortD()
{
  return (PIND & (1<<N));
}

