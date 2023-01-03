#include "midilooperapp.h"

MidiLooperApp::MidiLooperApp()
    : learnIdx(-1), recordingIdx(-1), isPlaying(true), prevIsPlaying(false)
{
}

void MidiLooperApp::Setup()
{
  midiTouchPad.ConfigureNote(0x09, 0x28, 0);
  midiTouchPad.ConfigureNote(0x09, 0x29, 1);  
  // FA FB
  midiTouchPad.ConfigureNote(0x09, 0x2A, 2);
  midiTouchPad.ConfigureNote(0x09, 0x2B, 3);
  // F1-4
  midiTouchPad.ConfigureNote(0x09, 0x30, 4);
  midiTouchPad.ConfigureNote(0x09, 0x31, 5);
  midiTouchPad.ConfigureNote(0x09, 0x32, 6);
  midiTouchPad.ConfigureNote(0x09, 0x33, 7);
  // btn1-8
  midiTouchPad.ConfigureNote(0x09, 0x24, 8);
  midiTouchPad.ConfigureNote(0x09, 0x25, 9);
  midiTouchPad.ConfigureNote(0x09, 0x26, 10);
  midiTouchPad.ConfigureNote(0x09, 0x27, 11);
  midiTouchPad.ConfigureNote(0x09, 0x2C, 12);
  midiTouchPad.ConfigureNote(0x09, 0x2D, 13);
  midiTouchPad.ConfigureNote(0x09, 0x2E, 14);
  midiTouchPad.ConfigureNote(0x09, 0x2F, 15);
  // toggle play/stop
  midiTouchPad.ConfigureController(0x0F, 0x73, 16);
  // reset
  midiTouchPad.ConfigureController(0x0F, 0x69, 17);
  // advance
  midiTouchPad.ConfigureController(0x0F, 0x68, 18);
}

void MidiLooperApp::Tick()
{
    ticker.Tick();
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

void MidiLooperApp::ProcessMidiIn(int maxBytes, HardwareSerial &serial)
{
    int cntr = 0; 
    uint8_t byte = 0x00;
    while(serialBuffer.Read(byte) && cntr<maxBytes)
    {
        ProcessMidiInByte(byte, serial);
        ++cntr;
    }
}

void MidiLooperApp::ProcessMidiInByte(uint8_t byte, HardwareSerial &serial)
{
    // serialBuffer.Read(byte);

    if (midiNoteParser.Parse(byte, message))
    {
        // dirty hack: channel == 0x09 => handle by touchpad
        if(0x09 == Channel(message))
        {
            midiTouchPad.Update(message);
            //TODO handle any button clicks here!!
        }
        // only used when recording or learning track
        else if (0 <= learnIdx)
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
