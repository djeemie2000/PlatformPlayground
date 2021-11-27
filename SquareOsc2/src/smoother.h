#pragma once
#include <mbed.h>

template<class T, int N>
class Smoother
{
public:
 Smoother(T initialValue = 0)
  : m_Value(initialValue)
 {}

 T smooth(T value)
 {
     // for ex. N=3 corresponds to
     // m_Value = (7*m_Value + value)/8;
     //         = (8*m_Value - m_Value + value )/8
     //         = (m_Value<<3 - m_Value + value) >> 3;
     m_Value = ((m_Value<<N) -m_Value + value)>>N;

     return m_Value;
 }

 T m_Value;
};
