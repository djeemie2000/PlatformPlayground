#pragma once

#include <mbed.h>

class MidiNoteStepper
{
public:
    enum EState {
        Off,
        NoteOn,
        On,
        NoteOff
    };

    MidiNoteStepper();

    void Clear();
    void AddStep(uint8_t MidiNote, uint8_t Velocity);
    void AddSkipStep();

    void Tick(int Gate);

    int CurrStep() const;
    int NumSteps() const;
    bool Skip() const;
    uint8_t Velocity() const;
    uint8_t MidiNote() const;
    bool Gate() const;
    EState CurrState() const;
    
private:
    void Advance();
    
    struct Step 
    {
        uint8_t MidiNote{0};
        bool Gate{false};
        uint8_t Velocity{0};
    };

    static const int Capacity = 64;

    int m_NumSteps{0};
    int m_CurrStep{0};
    Step m_Step[Capacity];
    int m_Gate{0};
    int m_PrevGate{0};
};
