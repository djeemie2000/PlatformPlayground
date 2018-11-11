#pragma once

#include <mbed.h>
#include "GateIn.h"
#include "GateTrack.h"

class CommonState;
class MidiHandler;

class TrackController
{
public:
    TrackController(PinName /*btnPin*/, MidiHandler& midiHandler, uint8_t MidiNote, uint8_t MidiChannel);

    void Tick(const CommonState& commonState, int btn);
    void SetPattern(uint32_t pattern);//presets, ...

    //for display:
    uint32_t GetDisplayPattern() const;
    uint32_t GetPattern() const;

private:
    GateState m_TrackBtn;
    GateTrackPlayer m_Player;
    uint8_t m_MidiChannel;
};
