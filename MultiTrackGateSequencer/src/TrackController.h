#pragma once

#include <mbed.h>
#include "GateIn.h"
#include "GateTrack.h"

class CommonState;
class MidiHandler;
class DigitalOutMatrix;

class TrackController
{
public:
    TrackController(MidiHandler& midiHandler, uint8_t MidiNote, uint8_t MidiChannel, int trackIdx, DigitalOutMatrix& ledMatrix);

    void Tick(const CommonState& commonState, int btn);
    void SetPattern(uint32_t pattern);//presets, ...

private:
    void SetStep(int step);
    void ClearStep(int step);

    GateState m_TrackBtn;
    GateTrackPlayer m_Player;
    uint8_t m_MidiChannel;
    int m_TrackIdx;
    DigitalOutMatrix& m_LedMatrix;
};
