#pragma once

#include <Arduino.h>
#include "GateHandler.h"
#include "GSData.h"

// Wrapper around GSTrack struct with (member) functions
class Track
{
public:
    Track(GSTrack* track);

    void Mute(bool mute);
    bool IsMuted() const;

    void SetStep(int step);
    void ClearStep(int step);
    bool GetStep(int step) const;

    void SetNumSteps(int numSteps);
    int GetNumSteps() const;

    GSTrack* m_Track;
};

class TrackPlayer
{
public:
    TrackPlayer(GateHandler& handler, Track& track);

    void PlayOn();
    void PlayOff();

    void StepOn();
    void StepOff();

    void ResetStep();

    int GetCurrentStep() const;//position of current step
    int GetNextStep() const;//position of next step

private:
    int AdvanceStep(int step) const;
    
    GateHandler& m_Handler;
    Track& m_Track;

    bool m_NoteOn;
    int m_CurrentStep;
    bool m_ResetStep;
};
