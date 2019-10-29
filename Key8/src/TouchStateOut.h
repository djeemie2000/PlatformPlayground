#pragma once

#include "TouchOutState.h"

class TouchStateOut
{
public:
  TouchStateOut() {}//undefined pins!

  void Begin(int gatePin, int bit0Pin, int bit1Pin, int bit2Pin)
  {
    m_GateOutPin = gatePin;
    m_Bit0Pin = bit0Pin;
    m_Bit1Pin = bit1Pin;
    m_Bit2Pin = bit2Pin;

    pinMode(m_GateOutPin, OUTPUT);
    pinMode(m_Bit0Pin, OUTPUT);
    pinMode(m_Bit1Pin, OUTPUT);
    pinMode(m_Bit2Pin, OUTPUT);
  }

  void Write(const TouchOutState& touchState)
  {
    digitalWrite(m_GateOutPin, 0<touchState.numPressed?1:0);
    digitalWrite(m_Bit0Pin, (touchState.selectedPad&1)?1:0);
    digitalWrite(m_Bit1Pin, (touchState.selectedPad&2)?1:0);
    digitalWrite(m_Bit2Pin, (touchState.selectedPad&4)?1:0);
  }

  private:
    int m_GateOutPin;
    int m_Bit0Pin;
    int m_Bit1Pin;
    int m_Bit2Pin;
};
