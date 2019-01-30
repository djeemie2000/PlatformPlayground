#pragma once

#include <mbed.h>
#include "GateIn.h"
#include "TrackPlayer.h"
#include "ClockOutQuantizer.h"

class CommonState;
class MidiHandler;
class DigitalOutMatrix;

class TrackController
{
public:
    TrackController(MidiHandler& midiHandler, uint8_t MidiNote, uint8_t MidiChannel, int trackIdx, DigitalOutMatrix& ledMatrix);

    void Tick(const CommonState& commonState, int btn, int allBtn);
    void SetPattern(uint32_t pattern, int length);//presets, ...

private:
    void SetStep(int step);
    void ClearStep(int step);

    GateState m_TrackBtn;
    GateState m_AllTrackBtn;
    ClockOutState m_GateOut;
    TrackPlayer m_Player;
    int m_TrackIdx;
    DigitalOutMatrix& m_LedMatrix;
};