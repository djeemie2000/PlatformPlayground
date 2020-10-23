#pragma once

#include <Arduino.h>

class AnalogOutBank;
class DigitalOutBank;
class ShiftOutBank;

void PitchOut(AnalogOutBank& out, int idx, uint8_t midiNote);
void VelocityOut(AnalogOutBank& out, int idx, uint8_t velocity);
void CCOut(AnalogOutBank& out, int idx, uint8_t value);
void GateOut(DigitalOutBank& out, int idx, int gate);
void GateOut(ShiftOutBank& out, int idx, int gate);
