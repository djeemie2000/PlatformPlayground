#pragma once
#include <Arduino.h>


#include "SerialOut.h"
#include "Max7219Matrix.h"
#include "TestDigitalOutMatrix.h"
#include "TTP8229TouchPad.h"
#include "TestTouchPad.h"

// peripherals
struct Peripherals
{
  SerialOut serialOut;
  Max7219Matrix ledMatrix;
  TTP8229TouchPad touchPad;
  
  Peripherals()
   : serialOut()
   , ledMatrix(4, PIN_SPI_SS)
   , touchPad()
  {}
};

