#pragma once

#include <mbed.h>
#include "MidiHandler.h"

class TrackPlayer
{
public:
    TrackPlayer(MidiHandler& handler, int NumSteps = 16);

    void PlayOn();
    void PlayOff();

    void StepOn();
    void StepOff();

    void Mute(bool mute);
    bool IsMuted() const;

    void SetStep(int step);
    void ClearStep(int step);
    bool GetStep(int step) const;
    void SetNumSteps(int numSteps);

    int GetCurrentStep() const;//position of current step
    int GetNextStep() const;//position of next step

    void Learn(uint8_t MidiNote, uint8_t Channel);
    void LearnNote(uint8_t MidiNote);
    void LearnChannel(uint8_t Channel);

private:
    int AdvanceStep(int step) const;
    
    int m_NumSteps;
    MidiHandler& m_Handler;
    uint8_t m_Channel;
    uint8_t m_MidiNote;
    bool m_NoteOn;

    bool m_Muted;

    int m_CurrentStep;
    uint32_t m_Pattern;
};
