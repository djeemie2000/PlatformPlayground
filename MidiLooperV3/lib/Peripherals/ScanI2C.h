#pragma once
#include <Arduino.h>

class HardwareSerial;

void ScanI2C(HardwareSerial &debugSerial);

bool FindI2CDevice(uint8_t address);