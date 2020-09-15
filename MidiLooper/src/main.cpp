#include <Arduino.h>
#include <Wire.h>
#include "MidiParser.h"
#include "MidiLooper.h"
#include "MPR121TouchPad.h"
#include "ScanI2C.h"
#include "TestTouchPad.h"
#include "StopWatch.h"
#include "DigitalOutBank.h"

// RX1 TX1 PA10 PA9
// RX2 TX2 PA3 PA2
// RX3 TX3 PB11 PB10
static const int debugPin = PB15;
HardwareSerial serialDebug(PA10, PA9);
HardwareSerial serialMidi(PA3, PA2);
MidiParser midiParser;
MidiLooper midiLooper;
MPR121TouchPad touchPad;
DigitalOutBank recordingLeds(PA11, PA12, PA15, PB3, PB4, PB5);

void setup()
{
  // put your setup code here, to run once:
  pinMode(debugPin, INPUT_PULLUP); //pull down for debug
  serialDebug.begin(115200);
  midiLooper.begin(&serialMidi); //calls serialMidi.begin(31250);

  pinMode(PC13, OUTPUT); // clock led

  // setup I2C for MPR121 touchpad
  // Wire.begin() is called inside touchpad.begin()
  Wire.setSDA(PB11);   //SDA2
  Wire.setSCL(PB10);   //SCL2
  touchPad.Begin(PB0); //irq pin
  recordingLeds.begin();

  serialDebug.println("MidiLooper v0.3");
}

void readMidiIn(HardwareSerial &serialMidi, MidiParser &parser, MidiHandler &handler, int maxNumBytesRead = 3)
{
  // read midi in but limit # bytes for performance issues
  int numBytesIn = serialMidi.available();
  const int maxNumBytesIn = 3;
  if (maxNumBytesIn < numBytesIn)
  {
    numBytesIn = maxNumBytesIn;
  }
  while (0 < numBytesIn)
  {
    uint8_t byte = serialMidi.read();
    parser.Parse(byte, handler);
    --numBytesIn;
  }
}

void updateMidiLooper(MidiLooper &midiLooper, MPR121TouchPad &touchPad, DigitalOutBank &recordingLeds)
{
  const int MidiLearnPad = 0;
  const int ToggleRecordingPad = 1;
  const int UndoRecordingPad = 2;
  const int MutePad = 3;

  const int AllNotesOffPad = 4;
  const int MetronomePad = 5;

  const int Track0Pad = 6;

  touchPad.Read();

  // metronome!!
  if (touchPad.IsClicked(MetronomePad))
  {
    // => check which function(s) is pressed
    if (touchPad.Get(MidiLearnPad))
    {
      midiLooper.m_Metronome.StartMidiLearn();
    }
    if (touchPad.Get(MutePad))
    {
      midiLooper.m_Metronome.onToggleMuted();
    }
  }

  for (int idx = 0; idx < MidiLooper::NumTracks; ++idx)
  {
    // track is clicked?
    if (touchPad.IsClicked(Track0Pad + idx))
    {
      // => check which function(s) is pressed
      if (touchPad.Get(MidiLearnPad))
      {
        midiLooper.m_Track[idx].StartMidiLearn();
      }
      if (touchPad.Get(ToggleRecordingPad))
      {
        midiLooper.m_Track[idx].ToggleRecording();
      }
      if (touchPad.Get(UndoRecordingPad))
      {
        midiLooper.m_Track[idx].onUndo(midiLooper.m_MidiOut); //?
      }
      if (touchPad.Get(MutePad))
      {
        midiLooper.m_Track[idx].onToggleMuted(midiLooper.m_MidiOut);
      }
      if (touchPad.Get(AllNotesOffPad))
      {
        midiLooper.m_Track[idx].AllNotesOff(midiLooper.m_MidiOut);
      }
    }
  }

  for (int idx = 0; idx < MidiLooper::NumTracks; ++idx)
  {
    if (touchPad.Get(MutePad))
    {
      recordingLeds.set(idx, midiLooper.m_Track[idx].m_Muted ? 0 : 1);
    }
    else if (touchPad.Get(MidiLearnPad))
    {
      recordingLeds.set(idx, midiLooper.m_Track[idx].m_MidiLearn ? 1 : 0);
    }
    else
    {
      recordingLeds.set(idx, midiLooper.m_Track[idx].m_Recording ? 1 : 0);
    }
  }

  recordingLeds.update();
}

void loop()
{
  // put your main code here, to run repeatedly:

  // startup checks
  ScanI2C(serialDebug);
  // remove/limit in time test touchpad
  const int numRepeats = 5;
  TestTouchPad(touchPad, serialDebug, numRepeats);

  testDigitalOutBank(recordingLeds, 2);

  midiLooper.allNotesOff(); //?? all channels???

  // main loop:
  int debugCounter = 0;
  StopWatch debugTimer;
  debugTimer.start();

  //fake clock
  int clockCounter = 0;
  while (true)
  {
    //fake clock
    const int fakeClockPeriod = 125;
    if (clockCounter < fakeClockPeriod)
    {
      ++clockCounter;
    }
    else
    {
      clockCounter = 0;
    }
    int clock = clockCounter < (fakeClockPeriod / 2) ? 1 : 0;
    int reset = 0;

    midiLooper.onTick(clock, reset);
    digitalWrite(PC13, clock ? LOW : HIGH); // turn the LED on/off

    // read midi in but limit # bytes for performance issues
    readMidiIn(serialMidi, midiParser, midiLooper, 3);

    updateMidiLooper(midiLooper, touchPad, recordingLeds);

    ++debugCounter;
    if (debugCounter >= 1000) //TODO stopwatch 1 sec + # runs
    {
      debugTimer.stop();
      if (0 == digitalRead(debugPin))
      {
        // serial debug out : timing, ...
        serialDebug.print(debugTimer.read_ms()); //elapsed
        serialDebug.print(" ");
        serialDebug.println(millis());

        midiLooper.printState(serialDebug);
      }
      debugCounter = 0;
      debugTimer.start();
    }

    delay(1); //TODO check if needed when using external clock input
  }
}
