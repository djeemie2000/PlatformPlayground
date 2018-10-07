#pragma once

#include <mbed.h>

class MidiNoteState
{
public:
    MidiNoteState();

    void Tick();
    void NoteOn(uint8_t MidiNote, uint8_t Velocity);
    void NoteOn(uint8_t MidiNote, uint8_t Velocity, int Transpose);
    void NoteOff(uint8_t MidiNote, uint8_t Velocity);

    int NumPressed() const;
    bool NotePressed(int MidiNote) const;
    uint8_t Velocity(int MidiNote) const;
    uint32_t PressDuration(int MidiNote) const;
    int Transpose(int MidiNote) const;
    
private:
    struct State 
    {
        uint8_t MidiNote{0};
        bool Pressed{false};
        uint8_t Velocity{0};
        uint32_t TickOn{0};
        uint32_t TickOff{0};
        int Transpose{0};
    };

    static const int NumMidiNotes = 128;

    uint32_t m_Tick{0};
    State m_State[NumMidiNotes];
    int m_NumPressed{0};
};
