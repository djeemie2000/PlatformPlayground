#pragma once

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

  int printf(const char *format, ...)
  {
    va_list ap;
    va_start(ap, format);
    int retval = snprintf(buf, bufLength, format, ap);
    Serial.println(buf);
    return retval;
  }
private:
  static const int bufLength = 100;
  char buf[bufLength];
};
