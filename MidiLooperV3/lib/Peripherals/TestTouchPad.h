#pragma once

#include <Arduino.h>
#include "MPR121TouchPad.h"
#include "MultiTouchPad.h"
#include "keypad.h"

void PrintTouchPad(MPR121TouchPad &touchPad, HardwareSerial &debugSerial);

void PrintTouchPad(MultiTouchPad &touchPad, HardwareSerial &debugSerial);

void PrintTouchPad(KeyPad &touchPad, HardwareSerial &debugSerial);


void TestTouchPad(MPR121TouchPad &touchPad, HardwareSerial &debugSerial, int count = -1);

void TestTouchPad(MultiTouchPad &touchPad, HardwareSerial &debugSerial, int count = -1);

void TestTouchPad(KeyPad &touchPad, HardwareSerial &debugSerial, int count = -1);
