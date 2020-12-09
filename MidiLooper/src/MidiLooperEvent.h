#pragma once
#include <Arduino.h>

struct MidiLooperEvent
{
    static const uint8_t NoteOffVelocity = 0x00;

    uint8_t m_Step;
    uint8_t m_MidiNote;
    uint8_t m_Velocity; // 0x00 means note off
    uint8_t m_Layer;

    bool IsNoteOn() const;
    bool IsNoteOff() const;
    void SetNoteOn(uint8_t step, uint8_t midiNote, uint8_t velocity, uint8_t layer);
    void SetNoteOff(uint8_t step, uint8_t midiNote, uint8_t /*velocity*/, uint8_t layer);
};

