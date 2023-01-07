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

    // TODO 
    MidiLooperTickerParams tickerParams;//TODO load from storage?
    ticker.Begin(tickerParams);
}

void MidiLooperApp::ReadMidiIn(HardwareSerial &serial, HardwareSerial &serialDebug)
{
    while (serial.available())
    {
        uint8_t byte = serial.read();
        // filter out realtime clock messages for midi thru
        // also for parsing??
        if (!(0xF8 <= byte && byte <= 0xFC)) //! ShouldIgnore(byte)
        {
            // midi thru:
            serial.write(byte);

            // buffer for further processing
            serialBuffer.Write(byte);

            // debug
            serialDebug.println(byte, HEX);
        }
    }
}

void MidiLooperApp::ProcessMidiIn(int maxBytes, HardwareSerial &serial, HardwareSerial &serialDebug)
{
    int cntr = 0;
    uint8_t byte = 0x00;
    while (serialBuffer.Read(byte) && cntr < maxBytes)
    {
        ProcessMidiInByte(byte, serial, serialDebug);
        ++cntr;
    }
}

void MidiLooperApp::ProcessMidiInByte(uint8_t byte, HardwareSerial &serial, HardwareSerial &serialDebug)
{
    // serialBuffer.Read(byte);

    if (midiNoteParser.Parse(byte, message))
    {
        PrintVoiceMessage(message, serialDebug);

        if (HandleMidiTouchpad(serial))
        {
        }
        // only used when recording or learning track
        else if (NumTracks == learnIdx)
        {
            if (IsNoteOn(message))
            {
                metronome.LearnNoteOn(Channel(message), MidiNote(message), serial);   
                learnIdx = -1; // stop learning
            }
        }
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
                // TODO 'quantization' for note on:
                //  if clock on, record into current step
                //  if clock off, record into next step
                track[recordingIdx].RecordNoteOn(ticker.GetStep(), Channel(message), MidiNote(message), Velocity(message));

                // serialDebug.println("ROn");
                // serialDebug.println(ticker.GetStep());
            }
            else if (IsNoteOff(message))
            {
                // 'quantization' for note off:
                // always record into current step
                track[recordingIdx].RecordNoteOff(ticker.GetStep(), Channel(message), MidiNote(message), Velocity(message));

                // serialDebug.println("ROff");
                // serialDebug.println(ticker.GetStep());
            }
        }
    }
}

void MidiLooperApp::PlayTracksClockOn(int step, HardwareSerial &serial)
{
    metronome.PlayClockOn(ticker.GetTick(), ticker.GetBeat(), ticker.GetBar(), serial);
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        track[idx].PlayClockOn(step, serial);
    }
}

void MidiLooperApp::PlayTracksClockOff(int step, HardwareSerial &serial)
{
    metronome.PlayClockOff(serial);
    for (int idx = 0; idx < NumTracks; ++idx)
    {
        track[idx].PlayClockOff(step, serial);
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

    // TODO handle playing -> !isPlaying
    // => all notes off

    if (isPlaying)
    {
        // tick midi looper ticker (24PPQ)
        ticker.Tick();
    }

    // send midi clock at 24PPQ
    PlayMidiClock(serial);

    if (isPlaying)
    {
        // detect clock on<->off !!
        bool currClockOn = ticker.GetClockOn();
        if (currClockOn != prevClockOn)
        {
            // send midi notes upon clock off->on, upon clock on-> off
            int step = ticker.GetStep();
            if (currClockOn)
            {
                PlayTracksClockOn(step, serial);
            }
            else
            {
                PlayTracksClockOff(step, serial);
            }
        }
    }
}

void MidiLooperApp::DisplayTicker(DigitalOutMatrix &matrix)
{
    uint8_t row0 = 1 << ticker.GetTick();
    if (!ticker.GetClockOn())
    {
        row0 = 0x00;
    }
    uint8_t row1 = 1 << ticker.GetBeat();
    uint8_t row2 = 1 << ticker.GetBar();

    matrix.SetRow(row0, 5, 0);
    matrix.SetRow(row1, 6, 0);
    matrix.SetRow(row2, 7, 0);
    matrix.WriteRow(5);
    matrix.WriteRow(6);
    matrix.WriteRow(7);
}

void MidiLooperApp::DiplayTrackState(DigitalOutMatrix &matrix)
{
    // for now, only 8 tracks -> single row
    // TODO handle multiple rows
    uint8_t trackStateRow2 = 0x00;

    // learn / record ~ blink fast/slow
    // blink based on millis
    // fast blink mask 0x10 blink 0x40 slow blink 0x80
    unsigned long millies = millis();
    uint8_t counter = millies >> 2;
    bool blinkFast = counter & 0x10;
    bool blinkNormal = counter & 0x40;

    for (int tr = 0; tr < 8; ++tr)
    {
        if (tr == learnIdx)
        {
            if (blinkFast)
            {
                bitSet(trackStateRow2, tr);
            }
            else
            {
                bitClear(trackStateRow2, tr);
            }
        }
        else if (tr == recordingIdx)
        {
            if (blinkNormal)
            {
                bitSet(trackStateRow2, tr);
            }
            else
            {
                bitClear(trackStateRow2, tr);
            }
        }
        else if (track[tr].GetPlayMute())
        {
            bitSet(trackStateRow2, tr);
        }
        else
        {
            bitClear(trackStateRow2, tr);
        }
    }

    matrix.SetRow(trackStateRow2, 2, 0);
    matrix.WriteRow(2);
}

void MidiLooperApp::DiplayMetronomeState(DigitalOutMatrix &matrix)
{
    uint8_t trackStateRow3 = 0x00;

    // learn / record ~ blink fast/slow
    // blink based on millis
    // fast blink mask 0x10 blink 0x40 slow blink 0x80
    unsigned long millies = millis();
    uint8_t counter = millies >> 2;
    bool blinkFast = counter & 0x10;

    if (NumTracks == learnIdx)
    {
        if (blinkFast)
        {
            bitSet(trackStateRow3, 7);
        }
    }
    else if (metronome.GetPlayMute())
    {
        bitSet(trackStateRow3, 7);
    }

    matrix.SetRow(trackStateRow3, 3, 0);
    matrix.WriteRow(3);
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
            // we allow only one learn or recording track at a time
            recordingIdx = -1;
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
            // we allow only one learn or recording track at a time
            learnIdx = -1;
            if (recordingIdx == idxTrack)
            {
                // toggle recording on -> off
                recordingIdx = -1;
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

void MidiLooperApp::HandleMetronomeBtnInput(bool trackClicked, bool learnPressed, bool recordPressed, bool undoPressed, HardwareSerial &serial)
{
    if (trackClicked)
    {
        if (learnPressed)
        {
            // we allow only one learn or recording track at a time
            recordingIdx = -1;
            const int idxTrack = NumTracks; // 1 above highest track index
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
        else if(recordPressed)
        {
            // ignore recording
        }
        else if(undoPressed)
        {
            // ignore undo
        }
        else
        {
            // toggle play/mute
            bool currentPlayMute = metronome.GetPlayMute();
            metronome.SetPlayMute(!currentPlayMute, serial);
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
    if (resetClicked)
    {
        ticker.Reset();
    }
}

bool MidiLooperApp::HandleMidiTouchpad(HardwareSerial &serial)
{
    // dirty hack: channel == 0x09 <=> handle by touchpad
    if (0x09 == Channel(message) || IsController(message))
    {
        midiTouchPad.Update(message);
        // TODO handle any button clicks here!!
        bool learnPressed = midiTouchPad.Get(0);
        bool recordPressed = midiTouchPad.Get(1);
        bool undoPressed = midiTouchPad.Get(2);

        HandleMetronomeBtnInput(midiTouchPad.IsClicked(7),
                                learnPressed, recordPressed, undoPressed, serial);

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