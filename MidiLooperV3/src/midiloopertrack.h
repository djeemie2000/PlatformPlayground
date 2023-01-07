#pragma once
#include <Arduino.h>
#include "midilooperitem.h"
#include "midilooperitemrefs.h"
#include "midinotemask.h"


class MidiLooperTrack
{
public:
    static const int Capacity = 256;

    MidiLooperTrack();

    bool LearnNoteOn(uint8_t channel, HardwareSerial& serial);//return true if learned, false if not
    
    bool RecordNoteOn(int step, uint8_t channel, uint8_t midinote, uint8_t velocity);
    bool RecordNoteOff(int step, uint8_t channel, uint8_t midinote, uint8_t velocity);
    bool Undo(HardwareSerial& serial);

    bool SetPlayMute(bool playMute, HardwareSerial& serial);
    bool GetPlayMute() const { return m_PlayMute;  }
    bool PlayClockOn(int step, HardwareSerial& serial);
    bool PlayClockOff(int step, HardwareSerial& serial);

    bool AllNotesOff(HardwareSerial& serial);

    void PrintDebug(HardwareSerial& serialDebug);

private:
    uint8_t m_Channel;
    MidiLooperItem m_Items[Capacity];
    MidiLooperItemRefs m_ItemRefs[Capacity];
    int m_Size;
    MidiNoteMask m_MidiNoteMask;
    bool m_PlayMute;
};
