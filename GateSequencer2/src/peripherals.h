#pragma once
#include <Arduino.h>


#include "Max7219Matrix.h"
#include "TestDigitalOutMatrix.h"
#include "TTP8229TouchPad.h"
#include "TestTouchPad.h"
#include "midinoteparser.h"
#include "miditouchpad.h"

// peripherals
struct Peripherals
{
  Max7219Matrix ledMatrix;
  MidiNoteParser midiParser;
  MidiTouchPad midiTouchPad;
  
  Peripherals()
   : ledMatrix(4, PIN_SPI_SS)
   , midiParser()
   , midiTouchPad()
  {}
};

