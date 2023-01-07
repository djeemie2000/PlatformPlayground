#pragma once
#include <Arduino.h>

#include "midinotemask.h"

class MidiLooperMetronome
{
public:
    MidiLooperMetronome();

    bool LearnNoteOn(uint8_t channel, uint8_t midiNote, HardwareSerial &serial); // return true if learned, false if not

    bool SetPlayMute(bool playMute, HardwareSerial& serial);
    bool GetPlayMute() const { return m_PlayMute;  }
    bool PlayClockOn(int tick, int beat, int bar, HardwareSerial& serial);
    bool PlayClockOff(HardwareSerial& serial);

    bool AllNotesOff(HardwareSerial& serial);

private:
    uint8_t m_Channel;
    uint8_t m_BaseNote;

    MidiNoteMask m_MidiNoteMask;
    bool m_PlayMute;

    uint8_t m_LastChannel;
    uint8_t m_LastNote;
};
