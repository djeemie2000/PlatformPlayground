#pragma once
#include <Arduino.h>
#include "orderedlist.h"
#include "midivoicemessage.h"
#include "midinotestack.h"

class MidiVoiceMessage;
class MidiOut;

//TODO MidiOut class
class MidiFreeLooperTrack
{
public:
    MidiFreeLooperTrack();

    void OnReset(MidiOut& midiOut);//TODO midiOut
    void OnTogglePlayMute(MidiOut& midiOut);//TODO midi out
    void OnToggleRecording();
    void OnMidiClock(MidiOut& midiOut);
    void OnMidiNoteEvent(MidiVoiceMessage& message);

private:
    void AllNotesOff(MidiOut& midiOut);
    void MessageOut(MidiOut& midiOut, MidiVoiceMessage& message);

    bool m_PlayMute;
    int m_RecordArm;
    bool m_ResetFlag;
    int m_Length24PPQ;
    int m_Counter24PPQ;
    OrderedList<128, MidiVoiceMessage> m_Events;
    MidiNoteStack m_NoteStack;
};
