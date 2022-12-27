#pragma once
#include <Arduino.h>

#include "midiloopertrack.h"
#include "midivoicemessage.h"
#include "midinoteparser.h"
#include "serialbuffer.h"

struct MidiLooperApp
{
    static const int NumTracks = 8;

    SerialBuffer<100> serialBuffer;
    MidiLooperTrack track[NumTracks];
    MidiNoteParser midiNoteParser;
    MidiVoiceMessage message;
    int learnIdx;
    int recordingIdx;
    bool isPlaying;
    bool prevIsPlaying;

    MidiLooperApp();

    void ReadMidiIn(HardwareSerial &serial);

    void ProcessMidiInByte(uint8_t byte, HardwareSerial &serial);

    void PlayTracksClockOn(HardwareSerial &serial);

    void PlayTracksClockOff(HardwareSerial &serial);

    // TODO support manual advance for step-alike recording??

    void PlayMidiClock(HardwareSerial &serial);

    //  per track:
    //    handle toggle/change learning
    //    handle toggle/change recording?
    //    handle undo,
    //    handle toggle playmute,
    void HandleTrackBtnInput(int idxTrack, bool trackClicked, bool learnPressed, bool recordPressed, bool undoPressed, HardwareSerial &serial);
};
