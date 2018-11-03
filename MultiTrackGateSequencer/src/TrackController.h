#pragma once

#include <mbed.h>
#include "GateIn.h"
#include "GateTrack.h"

class CommonState;
class MidiHandler;

class TrackController
{
public:
    TrackController(PinName btnPin, MidiHandler& midiHandler, uint8_t MidiNote, uint8_t MidiChannel);

    void Tick(CommonState& commonState);
    void SetPattern(uint32_t pattern);//presets, ...

    //for display:
    uint32_t GetDisplayPattern() const;

private:
    GateIn m_TrackBtn;
    GateTrackPlayer m_Player;
    uint8_t m_MidiChannel;
};
