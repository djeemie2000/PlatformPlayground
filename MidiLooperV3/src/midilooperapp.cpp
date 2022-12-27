#include "midilooperapp.h"

MidiLooperApp::MidiLooperApp()
    : learnIdx(-1), recordingIdx(-1), isPlaying(true), prevIsPlaying(false)
{
}

void MidiLooperApp::ReadMidiIn(HardwareSerial &serial)
{
    while (serial.available())
    {
        uint8_t byte = serial.read();
        // filter out realtime clock messages for midi thru
        // also for parsing??
        if (!(0xF8 <= byte && byte <= 0xFC))
        {
            // midi thru:
            serial.write(byte);

            // buffer for further processing
            serialBuffer.Write(byte);
        }
    }
}

void MidiLooperApp::ProcessMidiInByte(uint8_t byte, HardwareSerial &serial)
{
    // serialBuffer.Read(byte);

    if (midiNoteParser.Parse(byte, message))
    {
        // only used when recording or learning track
        if (0 <= learnIdx)
        {
            if (IsNoteOn(message))
            {
                track[learnIdx].LearnNoteOn(Channel(message), serial);
                learnIdx = -1; // stop learning
            }
        }
        else if (0 <= recordingIdx)
        {
            if (IsNoteOn(message))
            {
                track[recordingIdx].RecordNoteOn(Channel(message), MidiNote(message), Velocity(message), serial);
            }
            else if (IsNoteOff(message))
            {
                track[recordingIdx].RecordNoteOff(Channel(message), MidiNote(message), Velocity(message), serial);
            }
        }
    }
}

void MidiLooperApp::PlayTracksClockOn(HardwareSerial &serial)
{
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        track[idx].PlayClockOn(0, serial); // TODO step
    }
}

void MidiLooperApp::PlayTracksClockOff(HardwareSerial &serial)
{
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        track[idx].PlayClockOff(0, serial); // TODO step
    }
}

// TODO support manual advance for step-alike recording??

void MidiLooperApp::PlayMidiClock(HardwareSerial &serial)
{
    // send midi clock (24 PPQ) -> send start/stop when appropriate!
    if (isPlaying && !prevIsPlaying)
    {
        serial.write(0xFA); // midi start
        serial.write(0xF8); // midi clock
    }
    else if (!isPlaying && prevIsPlaying)
    {
        serial.write(0xFC); // midi stop
    }
    else if (isPlaying)
    {
        serial.write(0xF8); // midi clock
    }
    prevIsPlaying = isPlaying;
}

//  per track:
//    handle toggle/change learning
//    handle toggle/change recording?
//    handle undo,
//    handle toggle playmute,
void MidiLooperApp::HandleTrackBtnInput(int idxTrack, bool trackClicked, bool learnPressed, bool recordPressed, bool undoPressed, HardwareSerial &serial)
{
    if (trackClicked)
    {
        if (learnPressed)
        {
            if (learnIdx == idxTrack)
            {
                // toggle learn on -> off
                learnIdx = -1;
            }
            else
            {
                // no need to check if previously -1 or some other track
                learnIdx = idxTrack;
            }
        }
        else if (recordPressed)
        {
            if (recordingIdx == idxTrack)
            {
                // toggle learn on -> off
                learnIdx = -1;
            }
            else
            {
                // no need to check if previously -1 or some other track
                recordingIdx = idxTrack;
            }
        }
        else if (undoPressed)
        {
            track[idxTrack].Undo(serial);
        }
        else
        {
            // toggle play/mute
            bool currentPlayMute = track[idxTrack].GetPlayMute();
            track[idxTrack].SetPlayMute(!currentPlayMute, serial);
        }
    }
}
