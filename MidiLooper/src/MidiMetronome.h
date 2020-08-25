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
    //TODO learn channel base note

private:
    uint8_t m_MidiChannel;
    uint8_t m_BaseNote;
    int m_Divider;
    int m_Divider2;
    bool m_Playing;

    uint8_t m_LastNote;
};
