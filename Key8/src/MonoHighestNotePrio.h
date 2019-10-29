#pragma once

#include "TouchInState.h"
#include "TouchStateOut.h"

struct MonoHighestNotePrio
{
  TouchOutState m_OutState;

  MonoHighestNotePrio();
  void update(const TouchInState& inState);
};

MonoHighestNotePrio::MonoHighestNotePrio() : m_OutState()
{}

void MonoHighestNotePrio::update(const TouchInState& inState)
{
  m_OutState.numPressed = 0;//reset num pressed count
  // hold selected pad when none are pushed
  // highest pad priority => count up and set selected
  for(int pad = 0; pad<TouchInState::Size; ++pad)
  {
    if(inState.m_State[pad])
    {
      ++m_OutState.numPressed;
      m_OutState.selectedPad = pad;
    }
  }
}
