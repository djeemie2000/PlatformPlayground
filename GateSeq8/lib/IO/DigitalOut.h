#pragma once

#include <arduino.h>

class DigitalOut
{
public :
  DigitalOut(uint8_t pin) : m_Pin(pin)
  {
      pinMode(m_Pin, OUTPUT);
  }

  void write(int value)
  {
    digitalWrite(m_Pin, value);
  }

  void operator=(int value)
  {
    digitalWrite(m_Pin, value);
  }
  
private:
  uint8_t m_Pin;
};
