#pragma once
#include <Arduino.h>

#include "midilooperticker.h"
#include "midiloopertrack.h"
#include "midivoicemessage.h"
#include "midinoteparser.h"
#include "serialbuffer.h"
#include "miditouchpad.h"

struct MidiLooperApp
{
    static const int NumTracks = 8;

    MidiLooperTicker ticker;
    SerialBuffer<100> serialBuffer;
    MidiLooperTrack track[NumTracks];
    MidiNoteParser midiNoteParser;
    MidiVoiceMessage message;
    MidiTouchPad midiTouchPad;//TODO replace by buttons on dvboard
    int learnIdx;
    int recordingIdx;
    bool isPlaying;
    bool prevIsPlaying;

    MidiLooperApp();

    void Setup();

    void Tick();

    void ReadMidiIn(HardwareSerial &serial);

    void ProcessMidiIn(int maxBytes, HardwareSerial &serial);

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

    // TODO
    // functions btn input : toggle play, reset?, load/save?
};
