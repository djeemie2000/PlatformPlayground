#pragma once
#include <Arduino.h>

#include "MidiOut.h"
#include "MidiLooperTicker.h"

class MidiMetronome
{
public:
    MidiMetronome();

    void Start();
    void Stop();
    void OnTick(MidiLooperTicker &ticker, MidiOut &midiOut);
    void StartMidiLearn();
    void onToggleMuted()
    {
        m_Playing = !m_Playing;
    }
    void OnNoteOn(uint8_t channel, uint8_t midiNote, uint8_t velocity);

private:
    bool m_MidiLearn;
    uint8_t m_MidiChannel;
    uint8_t m_BaseNote;
    int m_TicksPerBeat;
    int m_BeatsPerBar;
    bool m_Playing;

    uint8_t m_LastChannel;
    uint8_t m_LastNote;
};
