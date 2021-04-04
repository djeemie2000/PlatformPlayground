#pragma once
#include <Arduino.h>
#include "MidiOut.h"
#include "MidiNoteStack.h"
#include "MidiLooperTicker.h"
#include "MidiLooperEvent.h"

class MidiLooperTrack
{
public:
    MidiLooperTrack();

    void onUndo(MidiOut &midiOut);
    void onToggleMuted(MidiOut &midiOut);

    void ToggleRecording();
    void StartRecording();
    void StopRecording();
    void ToggleMidiLearn();
    void StopMidiLearn();

    void onTick(const MidiLooperTicker &ticker, MidiOut &midiOut);
    void onNoteOn(const MidiLooperTicker &ticker, MidiOut &midiOut, uint8_t midiChannel, uint8_t midiNote, uint8_t velocity);
    void onNoteOff(const MidiLooperTicker &ticker, uint8_t midiChannel, uint8_t midiNote, uint8_t velocity);
    void AllNotesOff(MidiOut &midiOut);

    void printItems(HardwareSerial &serial);
    void printState(HardwareSerial &serial);

public:
    uint8_t m_MidiChannel;
    bool m_MidiLearn;
    bool m_Recording;
    bool m_Muted;

private:
    uint8_t m_CurrLayer;
    int m_CurrLayerSize;

    MidiNoteStack m_NoteStack;

    static const int EventCapacity = 192;//192
    MidiLooperEvent m_Events[EventCapacity];
    int m_NumEvents;

    static const int StepCapacity = 128;//4*4*8
    static const int StepDepth = 8;
    uint8_t m_StepSize[StepCapacity];
    uint8_t m_StepEventIndex[StepCapacity][StepDepth];
};
