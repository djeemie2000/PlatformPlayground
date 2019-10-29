#pragma once

#include "TouchInState.h"
#include "TouchStateOut.h"

struct Arp
{
  TouchOutState m_OutState;
  bool m_Gate;
  
  Arp();
  void update(const TouchInState& inState);
};

Arp::Arp() : m_OutState(), m_Gate(false)
{}

void Arp::update(const TouchInState& inState)
{
  bool advance = inState.m_Gate && !m_Gate;
  m_OutState.numPressed  = inState.m_Gate?1:0;
  m_Gate = inState.m_Gate;

  if(advance)
  {
    // advance outState to next selected 
    int selected = m_OutState.selectedPad;
    for(int idx = 1; idx<TouchInState::Size; ++idx)
    {
      ++selected;
      if(TouchInState::Size<=selected)
      {
        selected = 0;
      }
      if(inState.m_State[selected])
      {
        m_OutState.selectedPad = selected;
        break;
      }
    }
  }
}

