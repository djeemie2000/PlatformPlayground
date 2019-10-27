#pragma once

struct TouchInState
{
public:
 static const int Size = 8;//TODO template or dynamic size

 TouchInState() 
 : m_Gate(false)
 {
   for(int idx = 0; idx<Size; ++idx)
   {
     m_State[idx]= false;
  }
}

 bool m_Gate;
 bool m_State[Size];
};
