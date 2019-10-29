#pragma once

struct TouchOutState
{
  int numPressed;
  int selectedPad;

  TouchOutState() 
  : numPressed(0)
  , selectedPad(0)
  {}
};
