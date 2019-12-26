#pragma once

#include <mbed.h>
#include "GateHandler.h"

struct GSTrack
{
    uint32_t m_Pattern;
    int m_NumSteps;
    bool m_Muted;
    float m_GateDuration;
};

// Wrapper around GSTrack struct with (member) functions
class Track
{
public:
    Track(int NumSteps = 16);

    void Mute(bool mute);
    bool IsMuted() const;

    void SetStep(int step);
    void ClearStep(int step);
    bool GetStep(int step) const;

    void SetNumSteps(int numSteps);
    int GetNumSteps() const;

    GSTrack m_Track;
};

class TrackPlayer
{
public:
    TrackPlayer(GateHandler& handler, Track& track);

    void PlayOn();
    void PlayOff();

    void StepOn();
    void StepOff();

    // void Mute(bool mute);
    // bool IsMuted() const;

    // void SetStep(int step);
    // void ClearStep(int step);
    // bool GetStep(int step) const;
    // void SetNumSteps(int numSteps);
    void ResetStep();

    int GetCurrentStep() const;//position of current step
    int GetNextStep() const;//position of next step

private:
    int AdvanceStep(int step) const;
    
//    int m_NumSteps;
    GateHandler& m_Handler;
    Track& m_Track;

    bool m_NoteOn;
  //  bool m_Muted;
    int m_CurrentStep;
    bool m_ResetStep;
    //uint32_t m_Pattern;
};
