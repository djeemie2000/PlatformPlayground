#pragma once
#include <Arduino.h>

template<class T>
class ClockOutState
{
public:
  ClockOutState()
   : m_State(0)
   , m_Counter(0)
   , m_OnTicks(1)
   , m_Period(2)
   {}

  void Configure(int onTicks, int period)
  {
    m_OnTicks = onTicks;
    m_Period = period;
  }

   void Reset()
   {
     m_Counter = 0;
     m_State = (m_Counter<m_OnTicks) ? 1 : 0;
   }

   void Tick()
   {
     m_State = (m_Counter<m_OnTicks) ? 1 : 0;

     ++m_Counter;
     if(m_Period<=m_Counter)
     {
       m_Counter = 0;
     }
   }

  int Get() const
  {
     return m_State;
  }

private:

  int m_State;
  T m_Counter;
  T m_OnTicks;
  T m_Period;
};
