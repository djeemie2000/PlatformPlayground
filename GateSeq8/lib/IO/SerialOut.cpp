#include "SerialOut.h"

int SerialOut::printf(const char *format, ...)
  {
    va_list arg;
    va_start(arg, format);
    int retval = vsnprintf(buf, bufLength, format, arg);
    Serial.println(buf);
    va_end(arg);
    return retval;
  }

