#ifndef SERIAL_BUFFER_H_INCLUDE
#define SERIAL_BUFFER_H_INCLUDE

#include <Arduino.h>

template<int Capacity>
class SerialBuffer
{
public:
  SerialBuffer() : m_WritePos(0), m_ReadPos(0)
  {

  }

  bool Write(uint8_t byte)
  {
    m_Buffer[m_WritePos] = byte;
    IncreasePos(m_WritePos);
    //it(m_WritePos == m_ReadPos ) => overflow!!!!
    return true;
  }

  bool Read(uint8_t& byte)
  {
    if(m_ReadPos == m_WritePos)
    {
      return false;
    }
    byte = m_Buffer[m_ReadPos];
    IncreasePos(m_ReadPos);
    return true;
  }

private:
  void IncreasePos(int& pos)
  {
    ++pos;
    if(Capacity<=pos)
    {
      pos = 0;
    }
  }

  uint8_t m_Buffer[Capacity];
  int m_WritePos;
  int m_ReadPos;
};

#endif /* end of include guard: SERIAL_BUFFER_H_INCLUDE */
