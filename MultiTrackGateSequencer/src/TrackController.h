#pragma once

#include <mbed.h>
#include "GateIn.h"
#include "TrackPlayer.h"
#include "ClockOutQuantizer.h"

class CommonState;
class GateHandler;
class DigitalOutMatrix;
class GSTrack;

class TrackController
{
public:
    TrackController(GateHandler& handler, int trackIdx, GSTrack* track);

    void Tick(const CommonState& commonState, int btn);
    void SetTrack(GSTrack* track);

    void Update(DigitalOutMatrix& ledMatrix);

private:
    GateState m_TrackBtn;
    ClockOutState m_GateOut;
    Track m_Track;
    TrackPlayer m_Player;
    int m_TrackIdx;
};
