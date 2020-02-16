#pragma once

#include <Arduino.h>
#include "../IO/SerialOut.h"
#include "TTP8229TouchPad.h"

void PrintTouchPad(TTP8229TouchPad& touchPad, SerialOut& debugSerial);

void TestTouchPad(TTP8229TouchPad& touchPad, SerialOut& debugSerial, int count = -1);
