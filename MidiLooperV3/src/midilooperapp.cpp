#include "midilooperapp.h"

MidiLooperApp::MidiLooperApp()
    : learnIdx(-1), recordingIdx(-1), isPlaying(true), prevIsPlaying(false)
{
}

void MidiLooperApp::Setup()
{
    // F1-F7
    midiTouchPad.ConfigureNote(0x09, 0x28, 0); // learn
    midiTouchPad.ConfigureNote(0x09, 0x29, 1); // record
    midiTouchPad.ConfigureNote(0x09, 0x2A, 2); // undo
    midiTouchPad.ConfigureNote(0x09, 0x2B, 3); //??
    midiTouchPad.ConfigureNote(0x09, 0x30, 4); //??
    midiTouchPad.ConfigureNote(0x09, 0x31, 5); // save?
    midiTouchPad.ConfigureNote(0x09, 0x32, 6); // load?
                                               //  metronome
    midiTouchPad.ConfigureNote(0x09, 0x33, 7);
    // track1-8
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
        if (!(0xF8 <= byte && byte <= 0xFC))//!ShouldIgnore(byte)
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
    while (serialBuffer.Read(byte) && cntr < maxBytes)
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
        if (HandleMidiTouchpad(serial))
        {
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

void MidiLooperApp::HandleTick(HardwareSerial &serial)
{
    bool prevClockOn = ticker.GetClockOn();

    // tick midi looper ticker (24PPQ)
    Tick();
    
    // send midi clock at 24PPQ
    PlayMidiClock(serial);
    
    // detect clock on<->off !! 
    bool currClockOn = ticker.GetClockOn();
    if(currClockOn != prevClockOn)
    {
      // send midi notes upon clock off->on, upon clock on-> off
      if(currClockOn)
      {
        PlayTracksClockOn(serial);
      }
      else
      {
        PlayTracksClockOff(serial);
      }
    }

}

void MidiLooperApp::DisplayTicker(DigitalOutMatrix& matrix)
{
    uint8_t row0 = 1<<ticker.GetTick();
    if(!ticker.GetClockOn())
    {
        row0 = 0x00;
    }
    uint8_t row1 = 1<<ticker.GetBeat();
    uint8_t row2 = 1<<ticker.GetBar();

    matrix.SetRow(row0, 5, 0);
    matrix.SetRow(row1, 6, 0);
    matrix.SetRow(row2, 7, 0);
    matrix.WriteRow(5);
    matrix.WriteRow(6);
    matrix.WriteRow(7);
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

void MidiLooperApp::HandleGlobalBtnInput(bool playStopClicked, bool resetClicked)
{
    if (playStopClicked)
    {
        isPlaying = !isPlaying;
    }
    // reset functionality while playing?
    if(resetClicked)
    {
        ticker.Reset();
    }
}

bool MidiLooperApp::HandleMidiTouchpad(HardwareSerial &serial)
{
    // dirty hack: channel == 0x09 <=> handle by touchpad
    if (0x09 == Channel(message))
    {
        midiTouchPad.Update(message);
        // TODO handle any button clicks here!!
        bool learnPressed = midiTouchPad.Get(0);
        bool recordPressed = midiTouchPad.Get(1);
        bool undoPressed = midiTouchPad.Get(2);

        for (int track = 0; track < NumTracks; ++track)
        {
            HandleTrackBtnInput(track, midiTouchPad.IsClicked(track + 8),
                                learnPressed, recordPressed, undoPressed, serial);
        }
        HandleGlobalBtnInput(midiTouchPad.IsClicked(16), midiTouchPad.IsClicked(17));

        return true;
    }
    // return true if message is handled by touchpad
    return false;
}