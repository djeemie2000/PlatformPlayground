#pragma once

#include <Arduino.h>
#include "Mpr121TouchPad.h"

void PrintTouchPad(MPR121TouchPad &touchPad, HardwareSerial &debugSerial);

void TestTouchPad(MPR121TouchPad &touchPad, HardwareSerial &debugSerial, int count = -1);
