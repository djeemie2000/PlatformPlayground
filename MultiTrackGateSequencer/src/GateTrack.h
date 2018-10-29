#pragma once

#include <mbed.h>
#include "MidiHandler.h"

class GateTrack
{
public:
    GateTrack(MidiHandler& handler, int NumSteps = 16);

    void PlayOn();
    void PlayOff();

    void StepOn();
    void StepOff();

    void Mute(bool mute);
    bool IsMuted() const;

    void SetCurrentStep();
    void ClearCurrentStep();
    int GetCurrentStep() const;
    uint32_t GetPattern() const;

    void Learn(uint8_t MidiNote, uint8_t Channel);


private:
    const int m_NumSteps;

    MidiHandler& m_Handler;
    uint8_t m_Channel;
    uint8_t m_MidiNote;
    bool m_NoteOn;

    bool m_Muted;

    int m_CurrentStep;
    uint32_t m_Pattern;
};
