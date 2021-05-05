#pragma once

#include <Arduino.h>

class AnalogOutBank;
class DigitalOutBank;
class LedOutBank;

void PitchOut(AnalogOutBank& out, int idx, uint8_t midiNote, uint8_t baseNote);
void VelocityOut(AnalogOutBank& out, int idx, uint8_t velocity);
void CCOut(AnalogOutBank& out, int idx, uint8_t value);
void GateOut(AnalogOutBank& out, int idx, int gate);

void GateOut(DigitalOutBank& out, int idx, int gate);

void LedOut(LedOutBank& out, int idx, int gate);
