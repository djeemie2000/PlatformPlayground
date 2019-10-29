#pragma once

#include "TouchInState.h"

struct ChordMem
{
  TouchInState m_State;
  int m_NumPressed;

  ChordMem();
  void update(const TouchInState& inState);
};

ChordMem::ChordMem() : m_State(), m_NumPressed(0) 
{
}

void ChordMem::update(const TouchInState& inState)
{
  int prevNumPressed = m_NumPressed;
  int m_NumPressed = 0;
  for(int pad = 0; pad<TouchInState::Size; ++pad)
  {
    if(inState.m_State[pad])
    {
      ++m_NumPressed;
    }
  }
  bool resetHold = 0==prevNumPressed && 0<m_NumPressed;

  for(int pad = 0; pad<TouchInState::Size; ++pad)
  {
    // hold any previously pressed until 'reset' by pressing a first key again
    if(inState.m_State[pad])
    {
      m_State.m_State[pad] = true;
    }
    else if(resetHold)
    {
      m_State.m_State[pad] = false;
    }
  }

  m_State.m_Gate = inState.m_Gate;
}
