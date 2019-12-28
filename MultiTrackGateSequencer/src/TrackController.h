#pragma once

#include <mbed.h>
#include "GateIn.h"
#include "TrackPlayer.h"
#include "ClockOutQuantizer.h"

class CommonState;
class GateHandler;
class DigitalOutMatrix;

class TrackController
{
public:
    TrackController(GateHandler& handler, int trackIdx, DigitalOutMatrix& ledMatrix);

    void Tick(const CommonState& commonState, int btn);
    void SetPattern(uint32_t pattern, int length);//presets, ...

private:
    void SetStep(int step);
    void ClearStep(int step);

    GateState m_TrackBtn;
    ClockOutState m_GateOut;
    Track m_Track;
    TrackPlayer m_Player;
    int m_TrackIdx;
    DigitalOutMatrix& m_LedMatrix;
};
