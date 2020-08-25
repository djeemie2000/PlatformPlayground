#include <Arduino.h>
#include "MidiOut.h"
#include "MidiLooperTicker.h"
#include "MidiLooperTrack.h"
#include "MidiParser.h"

class MidiLooper : public MidiHandler
{
public:
  MidiLooper() : m_MidiOut(), m_Ticker(), m_Track() {}

  void begin(HardwareSerial *midiSerial)
  {
    m_MidiOut.begin(midiSerial);
  }

  void onTick(int clock, int reset)
  {
    m_Ticker.onTick(clock, reset);
    m_Track.onTick(m_Ticker, m_MidiOut);
  }

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
  {
    m_MidiOut.NoteOn(Channel, MidiNote, Velocity);
    m_Track.onNoteOn(m_Ticker, Channel, MidiNote, Velocity);
  }
  void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
  {
    m_MidiOut.NoteOff(Channel, MidiNote, Velocity);
    m_Track.onNoteOff(m_Ticker, Channel, MidiNote, Velocity);
  }

  MidiOut m_MidiOut;
  MidiLooperTicker m_Ticker;
  MidiLooperTrack m_Track; //TODO multiple tracks
};

HardwareSerial SerialDebug(PA10, PA9);
HardwareSerial SerialMidi(PB11, PB10);
//MidiOut midiOut;
//MidiLooperTrack midiLooper;
MidiParser midiParser;
//MidiLooperTicker midiLooperTicker;
MidiLooper midiLooper;

void setup()
{
  // put your setup code here, to run once:
  SerialDebug.begin(115200);
  //SerialMidi.begin(31250);
  midiLooper.begin(&SerialMidi);

  pinMode(PC13, OUTPUT);
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

    SerialDebug.println(byte, HEX); //TODO debug ~~ DigitalIn

    --numBytesIn;
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

  allNotesOff(midiLooper.m_MidiOut, 0x00);

  int debugCounter = 0;

  //fake clock
  int clockCounter = 0;
  while (true)
  {
    //fake clock
    const int fakeClockPeriod = 200;
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
    // int numBytesIn = SerialMidi.available();
    // const int maxNumBytesIn = 3;
    // if (maxNumBytesIn < numBytesIn)
    // {
    //   numBytesIn = maxNumBytesIn;
    // }
    // while (0 < numBytesIn)
    // {
    //   uint8_t byte = SerialMidi.read();
    //   midiParser.Parse(byte, midiLooper);
    //   // TODO midiOut + MidiLooper should handle note on/off
    //   // TODO parser to MidiOut (midi thru!) + looper!
    //   SerialDebug.println(byte, HEX); //TODO debug ~~ DigitalIn
    //   --numBytesIn;
    // }

    // //TODO clock rising => update midi looper tracks
    // if (midiLooperTicker.clockIsRising())
    // {
    //   midiLooper.onClockTick(midiLooperTicker, midiOut);
    // }

    ++debugCounter;
    if (debugCounter >= 1000)
    {
      //TODO serial debug out = timing, ...
      SerialDebug.print("D: ");
      SerialDebug.println(millis()); //TODO elapsed
      debugCounter = 0;
    }

    delay(1); //TODO check if needed
  }
}
