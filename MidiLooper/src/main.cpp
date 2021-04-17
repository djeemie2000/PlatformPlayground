#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "MidiParser.h"
#include "MidiLooper.h"
#include "MPR121TouchPad.h"
#include "ScanI2C.h"
#include "TestTouchPad.h"
#include "Max7219Matrix.h"
#include "TestDigitalOutMatrix.h"
#include "StopWatch.h"
#include "DigitalOutBank.h"
#include "ButtonState.h"
#include "DigitalIn.h"
#include "AnalogIn.h"
#include "DigitalInBank.h"

MidiParser midiParser;
MidiLooper midiLooper;

struct DevBoard
{
  // RX1 TX1 PA10 PA9
  // RX2 TX2 PA3 PA2
  // RX3 TX3 PB11 PB10
  HardwareSerial serialDebug;//(PA10, PA9);
  static const int debugPin = PA8;// TODO debugIn DIn
  HardwareSerial serialMidi;//(PB11, PB10);
  // TODO membank
  DigitalInBank IOXP1;
  DigitalOutBank IOXP2;
  DigitalOutBank IOXP3;
  AnalogIn Pot1;
  AnalogIn Pot2;
  AnalogIn Pot3;
  MPR121TouchPad MPR121A;
  Max7219Matrix ledMatrix;//(1,PA4);//cs pin

  DevBoard()
   : serialDebug(PA10, PA9)
   , serialMidi(PB11, PB10)
   , IOXP1(PB15, PB14, PB13, PB12)
   , IOXP2(PB3, PA15, PA12, PA11)
   , IOXP3(PB7, PB6, PB5, PB4)
   , Pot1()
   , Pot2()
   , Pot3()
   , MPR121A()
   , ledMatrix(1,PA4)//cs pin
   {}

   void Begin()
   {
     pinMode(PC13, OUTPUT);// led builtin
     pinMode(debugPin, INPUT_PULLUP);

     serialDebug.begin(115200);
     serialMidi.begin(31250);

     IOXP1.begin();
     IOXP2.begin();
     IOXP3.begin();

     Pot1.begin(PA0);
     Pot2.begin(PA1);
     Pot3.begin(PA2);

     // setup I2C for MPR121 MPR121A
     // Wire.begin() is called inside MPR121A.begin()
     Wire.setSDA(PB9);//PB11);   //SDA2
     Wire.setSCL(PB8);//PB10);   //SCL2
     MPR121A.Begin(PB0); //irq pin
     //TODO autoDetect I2C : MPR121A memBank

      SPI.setSCLK(PA5);
      SPI.setMISO(PA6);
      SPI.setMOSI(PA7);
      //SPI CS external
      ledMatrix.Configure();     
   }

   void update()
   {
     //TODO!!!!!!!!!!!!!!
    IOXP1.update();
    IOXP2.update();
    IOXP3.update();

     Pot1.Read();
     Pot2.Read();
     Pot3.Read();
   }

   //TODO update() -> store in variable
   bool debugActive() const
   {
     return 0 == digitalRead(debugPin);
   }

   void LedOn()
   {
     digitalWrite(PC13, LOW);
   }

   void LedOff()
   {
     digitalWrite(PC13, HIGH);
   }
};

DevBoard devBoard;

ButtonState metronomePadState;
ButtonState trackPadState[MidiLooper::NumTracks];
ButtonState learnModePadState;
ButtonState recordingModePadState;
ButtonState playModePadState;

static const int LearnMode =0;
static const int RecordingMode = 1;
static const int PlayMode = 2;

void setup()
{
  // put your setup code here, to run once:
  //pinMode(debugPin, INPUT_PULLUP); //pull down for debug
  devBoard.Begin();
  midiLooper.begin(&devBoard.serialMidi); //calls serialMidi.begin(31250);

  delay(1000);
  devBoard.serialDebug.println("MidiLooper v0.5");
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

    //devBoard.serialDebug.println(byte, HEX);

    --numBytesIn;
  }
}

void updateMidiLooper(MidiLooper &midiLooper, MPR121TouchPad &touchPad, int& currentMode, Max7219Matrix& ledMatrix)
{
  const int LearnModePad = 8;
  const int RecordingModePad = 4;
  const int PlayModePad = 0;

  const int MetronomePad = 9;
  const int TrackPads[] = {5,1, 10,6,2, 11,7,3};
  //6 tracks + metronome
  const int recordingY[] = {1,2, 0,1,2, 0,1,2, 0};
  const int recordingX[] = {5,5, 6,6,6, 7,7,7, 5};
  const int playY[] = {6,7, 5,6,7, 5,6,7, 5};
  const int playX[] = {5,5, 6,6,6, 7,7,7, 5};
  
  touchPad.Read();

  unsigned long currMillis = millis();

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

  learnModePadState.update(currMillis, touchPad.Get(LearnModePad));
  recordingModePadState.update(currMillis, touchPad.Get(RecordingModePad));
  playModePadState.update(currMillis, touchPad.Get(PlayModePad));
  if(learnModePadState.LongPressed())
  {
    //serialDebug.println("learn pad long");
    midiLooper.m_Ticker.SetNumBars(1);// 2 bars
  }
  if(recordingModePadState.LongPressed())
  {
    //serialDebug.println("rec   pad long");
    midiLooper.m_Ticker.SetNumBars(2);// 4 bars
  }
  if(playModePadState.LongPressed())
  {
    //serialDebug.println("play  pad long");
    midiLooper.m_Ticker.SetNumBars(3);// 8 bars
  }


  // metronome update
  metronomePadState.update(currMillis, touchPad.Get(MetronomePad));
  if (metronomePadState.Tapped())
  {
    //serialDebug.println("metro pad tap");
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

  // tracks update
  for (int idx = 0; idx < MidiLooper::NumTracks; ++idx)
  {
    trackPadState[idx].update(currMillis, touchPad.Get(TrackPads[idx]));

    if(trackPadState[idx].Tapped())
    {
        // serialDebug.print("track pad ");
        // serialDebug.print(idx);
        // serialDebug.println(" tap");

        if(currentMode == LearnMode)
        {
          midiLooper.ToggleMidiLearn(idx);
        }
        else if(currentMode == RecordingMode)
        {
          midiLooper.ToggleRecording(idx);
        }
        else if(currentMode == PlayMode)
        {
          midiLooper.m_Track[idx].onToggleMuted(midiLooper.m_MidiOut);
        }
    }
    else if(trackPadState[idx].LongPressed())
    {
        // serialDebug.print("track pad ");
        // serialDebug.print(idx);
        // serialDebug.println(" long");

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

  // indicate current mode with leds (row 0)
  if(currentMode == LearnMode)
  {
    ledMatrix.Set(0,0);
    ledMatrix.Set(0,1);
    ledMatrix.Clear(0,3);
    ledMatrix.Clear(0,4);
    ledMatrix.Clear(0,6);
    ledMatrix.Clear(0,7);
  }
  else if(currentMode == RecordingMode)
  {
    ledMatrix.Clear(0,0);
    ledMatrix.Clear(0,1);
    ledMatrix.Set(0,3);
    ledMatrix.Set(0,4);
    ledMatrix.Clear(0,6);
    ledMatrix.Clear(0,7);
  }
  else if(currentMode == PlayMode)
  {
    ledMatrix.Clear(0,0);
    ledMatrix.Clear(0,1);
    ledMatrix.Clear(0,3);
    ledMatrix.Clear(0,4);
    ledMatrix.Set(0,6);
    ledMatrix.Set(0,7);
  }

  // indicate ticker state (which bar) (row 2)
  TickerState tickerState;
  midiLooper.m_Ticker.GetTickerState(tickerState);
  for(int col = 0; col<8; ++col)
  {
    if(col<tickerState.m_NumBars)
    {
      ledMatrix.Clear(2, col);
    }
    else
    {
      ledMatrix.Set(2, col);
    }
  }
  // blink on upon first tick of each beat
  if(tickerState.m_Tick==0)
  {
    ledMatrix.Set(2, tickerState.m_Bar);
  }

  bool blinkOn = (currMillis>>7) & 0x01;
  for (int idx = 0; idx < MidiLooper::NumTracks; ++idx)
  {
      // play leds on/off ~play/mute
      if(midiLooper.m_Track[idx].m_Muted)
      {
        ledMatrix.Clear(playX[idx], playY[idx]);
      }
      else
      {
        ledMatrix.Set(playX[idx], playY[idx]);        
      }
      
      // recording leds: blink if midi learn, on/off ~recording otherwise
      if(midiLooper.m_Track[idx].m_MidiLearn)
      { 
        if(blinkOn)
        {
            ledMatrix.Set(recordingX[idx], recordingY[idx]);
        }
        else
        {
          ledMatrix.Clear(recordingX[idx], recordingY[idx]);
        }      
      }
      else if(midiLooper.m_Track[idx].m_Recording)
      {
        ledMatrix.Set(recordingX[idx], recordingY[idx]);
      }
      else
      {
        ledMatrix.Clear(recordingX[idx], recordingY[idx]);
      }
  }

  // metronome
  //   play led ~~ playing 
  //   recording led blink if midi learn
  if(midiLooper.m_Metronome.IsPlaying())
  {
    ledMatrix.Set(playX[MidiLooper::NumTracks], playY[MidiLooper::NumTracks]);
  }
  else
  {
    ledMatrix.Clear(playX[MidiLooper::NumTracks], playY[MidiLooper::NumTracks]);        
  }
  
  if(midiLooper.m_Metronome.IsLearning() && blinkOn)
  {
      ledMatrix.Set(recordingX[MidiLooper::NumTracks], recordingY[MidiLooper::NumTracks]);
  }
  else
  {
    ledMatrix.Clear(recordingX[MidiLooper::NumTracks], recordingY[MidiLooper::NumTracks]);
  }    

  ledMatrix.WriteAll();
}

void loop()
{
  // put your main code here, to run repeatedly:

  // startup checks
  if(devBoard.debugActive())
  {
    ScanI2C(devBoard.serialDebug);

    const int numRepeats = 16;
    TestTouchPad(devBoard.MPR121A, devBoard.serialDebug, numRepeats);

    devBoard.serialDebug.println("test led matrix");
    testDigitalOutMatrix(devBoard.ledMatrix, 1);
  }

  // make sure all notes are off on all midi channels
  devBoard.serialDebug.println("all notes off");
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
  #ifdef FAKECLOCK
  int clockCounter = 0;
  #endif
  
  while (true)
  {
    //fake clock
    delay(1); //TODO check if needed when using external clock input

    #ifdef FAKECLOCK
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
    #endif

    #ifndef FAKECLOCK
    // clock + reset from digitalIn
    devBoard.IOXP1.update();
    int clock = devBoard.IOXP1.get(0);
    #endif
    
    midiLooper.onTick(clock);
    // turn the LED on/off
    if(clock)
    {
      devBoard.LedOn();
    }
    else
    { 
      devBoard.LedOff();
    }
   
    // read midi in but limit # bytes for performance issues
    readMidiIn(devBoard.serialMidi, midiParser, midiLooper, 3);

    updateMidiLooper(midiLooper, devBoard.MPR121A, currentMode, devBoard.ledMatrix);

    ++debugCounter;
    if (debugCounter >= 1000) //TODO stopwatch 1 sec + # runs
    {
      debugTimer.stop();
      if (devBoard.debugActive())
      {
        // serial debug out : timing, ...       
        devBoard.serialDebug.print(debugTimer.read_ms()); //elapsed
        devBoard.serialDebug.print(" ");
        devBoard.serialDebug.println(millis());

        //midiLooper.printState(serialDebug);
      }
      debugCounter = 0;
      debugTimer.start();
    }

  }
}
