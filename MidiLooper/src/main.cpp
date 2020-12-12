#include <Arduino.h>
#include <Wire.h>
#include "MidiParser.h"
#include "MidiLooper.h"
#include "MPR121TouchPad.h"
#include "ScanI2C.h"
#include "TestTouchPad.h"
#include "StopWatch.h"
#include "DigitalOutBank.h"
#include "ButtonState.h"

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
DigitalOutBank playLeds(PA0, PA1, PA4, PA5, PA6, PA7); 
ButtonState metronomePadState;
ButtonState trackPadState[MidiLooper::NumTracks];
//TODO clock reset inputs
//TODO play leds vs recording/learning leds

static const int LearnMode =0;
static const int RecordingMode = 1;
static const int PlayMode = 2;

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
  playLeds.begin();

  serialDebug.println("MidiLooper v0.5");
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

void updateMidiLooper(MidiLooper &midiLooper, MPR121TouchPad &touchPad, DigitalOutBank &recordingLeds, int& currentMode)
{
  const int LearnModePad = 8;
  const int RecordingModePad = 4;
  const int PlayModePad = 0;

  const int MetronomePad = 9;

  const int TrackPads[] = {10,6,2,11,7,3};

  touchPad.Read();

  if (touchPad.IsClicked(LearnModePad))
  {
    currentMode = LearnMode;
  }
  if (touchPad.IsClicked(RecordingModePad))
  {
    currentMode = RecordingMode;
  }
  if (touchPad.IsClicked(PlayModePad))
  {
     currentMode = PlayMode;
  }


  unsigned long currMillis = millis();

  // metronome!!
  metronomePadState.update(currMillis, touchPad.Get(MetronomePad));
  if (metronomePadState.Tapped())
  {
    // => check which function(s) is pressed
    if(currentMode == LearnMode)//if (touchPad.Get(LearnModePad))
    {
      midiLooper.m_Metronome.ToggleMidiLearn();
    }
    //if (touchPad.Get(PlayModePad))
    else if(currentMode == PlayMode)
    {
      midiLooper.m_Metronome.onToggleMuted();
    }
  }

  for (int idx = 0; idx < MidiLooper::NumTracks; ++idx)
  {
    trackPadState[idx].update(currMillis, touchPad.Get(TrackPads[idx]));

    if(trackPadState[idx].Tapped())
    {
        if(currentMode == LearnMode)
        {
          midiLooper.m_Track[idx].ToggleMidiLearn();
        }
        else if(currentMode == RecordingMode)
        {
          midiLooper.m_Track[idx].ToggleRecording();
        }
        else if(currentMode == PlayMode)
        {
          midiLooper.m_Track[idx].onToggleMuted(midiLooper.m_MidiOut);
        }
    }
    else if(trackPadState[idx].LongPressed())
    {
        if(currentMode == RecordingMode)
        {
          midiLooper.m_Track[idx].onUndo(midiLooper.m_MidiOut);
        }
        else if(currentMode == PlayMode)
        {
          midiLooper.m_Track[idx].AllNotesOff(midiLooper.m_MidiOut);
        }
    }
  }

  for (int idx = 0; idx < MidiLooper::NumTracks; ++idx)
  {
      // play leds on/off ~play/mute
      playLeds.set(idx, midiLooper.m_Track[idx].m_Muted ? 0 : 1);
      // recording leds: blink if midi learn, on/off ~recording otherwise
      if(midiLooper.m_Track[idx].m_MidiLearn)
      {
        recordingLeds.set(idx, (currMillis>>7) & 0x01);
      }
      else
      {
        recordingLeds.set(idx, midiLooper.m_Track[idx].m_Recording ? 1 : 0);
      }
 //   }
  }

  recordingLeds.update();
  playLeds.update();
}

void loop()
{
  // put your main code here, to run repeatedly:

  // startup checks
  ScanI2C(serialDebug);
  // remove/limit in time test touchpad
  const int numRepeats = 10;
  TestTouchPad(touchPad, serialDebug, numRepeats);

  for(int repeat = 0; repeat<4; ++repeat)
  {
    testDigitalOutBank(recordingLeds, 1);
    testDigitalOutBank(playLeds, 1);
  }

  // make sure all notes are off on all midi channels
  for(int ch =0; ch<16; ++ch)
  {
    midiLooper.m_MidiOut.allNotesOff(ch); 
  }

  // main loop:
  int debugCounter = 0;
  StopWatch debugTimer;
  debugTimer.start();

  int currentMode = LearnMode;

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
    //TODO clock + reset from digitalIn
    int clock = clockCounter < (fakeClockPeriod / 2) ? 1 : 0;
    int reset = 0;

    midiLooper.onTick(clock, reset);
    digitalWrite(PC13, clock ? LOW : HIGH); // turn the LED on/off

    // read midi in but limit # bytes for performance issues
    readMidiIn(serialMidi, midiParser, midiLooper, 3);

    updateMidiLooper(midiLooper, touchPad, recordingLeds, currentMode);

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
