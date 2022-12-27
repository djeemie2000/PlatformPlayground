#pragma once
#include <Arduino.h>

struct MidiLooperItemRefs
{
    static const int Capacity = 12;
    uint8_t refs[Capacity];
    uint8_t size;

    MidiLooperItemRefs();
    void AddReference(uint8_t idx);
    void RemoveReference();
};

