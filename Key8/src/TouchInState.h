#pragma once

struct TouchInState
{
public:
 static const int Size = 8;//TODO template or dynamic size

 TouchInState() 
 : m_Gate(false)
 , m_Num(0)
 {
   for(int idx = 0; idx<Size; ++idx)
   {
     m_State[idx]= false;
  }
}

 bool m_Gate;
 bool m_State[Size];
 int m_Num;
};
