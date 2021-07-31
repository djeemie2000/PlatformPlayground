#pragma once

#include <Arduino.h>
#include "MPR121TouchPad.h"
#include "MultiTouchPad.h"

void PrintTouchPad(MPR121TouchPad &touchPad, HardwareSerial &debugSerial);

void PrintTouchPad(MultiTouchPad &touchPad, HardwareSerial &debugSerial);

void TestTouchPad(MPR121TouchPad &touchPad, HardwareSerial &debugSerial, int count = -1);

void TestTouchPad(MultiTouchPad &touchPad, HardwareSerial &debugSerial, int count = -1);
