#pragma once
#include <Arduino.h>

template<class T>
class SingleShotOutState
{
public:
  SingleShotOutState()
   : m_State(0)
   , m_Counter(0)
   , m_OnTicks(1)
   {}

  void Configure(int onTicks)
  {
    m_OnTicks = onTicks;
  }

   void Reset()
   {
     m_Counter = 0;
     m_State = (m_Counter<m_OnTicks) ? 1 : 0;
   }

   void Tick()
   {
     m_State = (m_Counter<m_OnTicks) ? 1 : 0;

    // do not increment beyond m_OnTicks + 1, prevent flipping over when reaching max_int
     if(m_OnTicks<m_Counter)
     {
         ++m_Counter;
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
};
