#include <Arduino.h>
#include <Wire.h>
#include "MidiParser.h"
#include "MidiLooper.h"
#include "MPR121TouchPad.h"
#include "ScanI2C.h"
#include "TestTouchPad.h"

HardwareSerial SerialDebug(PA10, PA9);
HardwareSerial SerialMidi(PB11, PB10);
MidiParser midiParser;
MidiLooper midiLooper;
MPR121TouchPad touchPad;

void setup()
{
  // put your setup code here, to run once:
  SerialDebug.begin(115200);
  //SerialMidi.begin(31250);
  midiLooper.begin(&SerialMidi);

  pinMode(PC13, OUTPUT);

  // setup I2C for MPR121 touchpad
  // Wire.begin() is called inside touchpad.begin()
  Wire.setSDA(PB7);
  Wire.setSCL(PB6);
  touchPad.Begin(PB0); //irq pin

  SerialDebug.println("MidiLooper v0.2");
}

void ledOn()
{
  digitalWrite(PC13, LOW); // turn the LED on
  SerialDebug.println("On");
  //  uint8_t msg[3] = {0x90,0x30,0x60};
  //  SerialMidi.write(msg,3);
  midiLooper.m_MidiOut.NoteOn(0x00, 0x2D, 0x60);
}

void ledOff()
{
  digitalWrite(PC13, HIGH); // turn the LED off
  SerialDebug.println("Off");
  //  uint8_t msg[3] = {0x80,0x30,0x60};
  //  SerialMidi.write(msg,3);
  midiLooper.m_MidiOut.NoteOff(0x00, 0x2D, 0x60);
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

    // SerialDebug.print(byte, HEX); //TODO debug ~~ DigitalIn
    // SerialDebug.print(" ");

    --numBytesIn;
  }
}

void updateMidiLooper(MidiLooper &midiLooper, MPR121TouchPad &touchPad)
{
  const int MidiLearnPad = 0;
  const int ToggleRecordingPad = 1;
  const int UndoRecordingPad = 2;
  const int MutePad = 3;
  const int Track0Pad = 8;
  const int MetronomePad = 7;

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
        midiLooper.m_Track[idx].onToggleMuted(); //midiLooper.m_MidiOut); //?
      }
    }
  }
}

void loop()
{
  // put your main code here, to run repeatedly:

  // blinking LED
  // for(int i = 0; i<3; ++i)
  // {
  //  ledOn();
  //  delay(300);
  //  ledOff();
  //  delay(300);
  // }
  // for(int i = 0; i<2; ++i)
  // {
  //   ledOn();
  //   delay(800);
  //   ledOff();
  //   delay(800);
  // }
  ScanI2C(SerialDebug);
  // remove/limit in time test touchpad
  const int numRepeats = 5;
  TestTouchPad(touchPad, SerialDebug, numRepeats);

  allNotesOff(midiLooper.m_MidiOut, 0x00);

  int debugCounter = 0;

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
    readMidiIn(SerialMidi, midiParser, midiLooper, 3);

    updateMidiLooper(midiLooper, touchPad);

    ++debugCounter;
    if (debugCounter >= 1000)
    {
      //TODO serial debug out = timing, ...
      SerialDebug.print("D: ");
      SerialDebug.println(millis()); //TODO elapsed
      debugCounter = 0;

      midiLooper.printState(SerialDebug);
      //midiLooper.m_Track.printItems(SerialDebug);
    }

    delay(1); //TODO check if needed
  }
}
