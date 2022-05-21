#pragma once

#include <Arduino.h>

class SerialOut
{
public:
  SerialOut(){}

  void begin(unsigned long  baudrate)
  {
   Serial.begin(baudrate);
  }

  void print(const char* content)
  {
    Serial.print(content);
  }

  void println(const char* content)
  {
    Serial.println(content);
  }

  void println()
  {
    Serial.println();
  }

  int printf(const char *format, ...);
  // {
  //   va_list arg;
  //   va_start(arg, format);
  //   int retval = vsnprintf(buf, bufLength, format, arg);
  //   Serial.println(buf);
  //   va_end(arg);
  //   return retval;
  // }

//  int vprintf(const char* format, va_list args) 
//  {
//   int r = vsnprintf(buf, bufLength, format, args);
//   Serial.println(buf);
//   return r;
// }

private:
  static const int bufLength = 100;
  char buf[bufLength];
};
