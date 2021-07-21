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

// struct MidiLooperNoteEvent
// {
//     uint8_t m_MidiNote;
//     uint8_t m_Layer;
//     uint8_t m_StepOn;
//     uint8_t m_VelocityOn;
//     uint8_t m_StepOff;
//     uint8_t m_VelocityOff;// 0xFF means no note off
  
//     void SetNoteOn(uint8_t step, uint8_t midiNote, uint8_t velocity, uint8_t layer);
//     bool SetNoteOff(uint8_t step, uint8_t midiNote, uint8_t velocity);
//     bool HasNoteOff() const;
//     bool IsValid() const;
// };

// class MidiLooperStepStack
// {
// public:
//     MidiLooperStepStack();

//     bool OnNoteOn(uint8_t midiNote, uint8_t velocity, uint8_t layer);
//     bool OnNoteOff(uint8_t midiNote, uint8_t velocity, uint8_t layer);
//     void OnUndo(uint8_t layer);

// public:
//     static const int Capacity = 8;
//     static const uint8_t InvalidNote = 0xFF;
//     static const uint8_t NoteOffVelocity = 0x00;

//     uint8_t m_MidiNoteOn[Capacity];
//     uint8_t m_Velocity[Capacity];
//     uint8_t m_LayerOn[Capacity];
//     int m_SizeOn;

//     uint8_t m_MidiNoteOff[Capacity];
//     uint8_t m_LayerOff[Capacity];
//     int m_SizeOff;
// };
