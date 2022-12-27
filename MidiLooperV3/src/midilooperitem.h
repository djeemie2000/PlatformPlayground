#pragma once
#include <Arduino.h>

struct MidiLooperItem
{
    uint8_t midiNote;
    uint8_t velocity;
    int stepOn;
    int stepOff;
};
