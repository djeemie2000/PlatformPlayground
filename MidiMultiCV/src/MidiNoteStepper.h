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

    // record:
    void Clear();
    void AddStep(uint8_t MidiNote, uint8_t Velocity);
    void AddSkipStep();

    // play:
    void Tick(int Gate);
    void SetBaseNote(uint8_t MidiNote);

    // getters:
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
    //TODO m_BeginNote vs m_PlayNote
    int m_BaseNote{-1};
    int m_PlayBaseNote{-1};
};
