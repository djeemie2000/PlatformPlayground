#include <Arduino.h>
#include <Wire.h>
#include "ScanI2C.h"
#include "TestTouchPad.h"
#include "TestDigitalOutMatrix.h"
#include "MidiParser.h"
#include "MidiLooper.h"
#include "StopWatch.h"
#include "DigitalOutBank.h"
#include "ButtonState.h"
#include "DevBoard.h"

//SPIClass SPI2(HSPI);

#define FAKECLOCK 1

DevBoard devBoard;

//TODO midiLooperApp struct : button state + mode + midiLooper/midiParser
ButtonState metronomePadState;
ButtonState trackPadState[MidiLooper::NumTracks];
ButtonState learnModePadState;
ButtonState recordingModePadState;
ButtonState playModePadState;

static const int LearnMode =0;
static const int RecordingMode = 1;
static const int PlayMode = 2;

//uint8_t testBuffer[16*1024];
MidiParser midiParser;
MidiLooper midiLooper;


void setup() {
  // put your setup code here, to run once:
  devBoard.Begin();

  // SPI2.begin();
  // SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE3));
  delay(500); // Allow 500ms for the 8229BSF to get ready after turn-on

  midiLooper.begin(&devBoard.serialMidi); //calls serialMidi.begin(31250);

  delay(1000);
  devBoard.serialDebug.println("MidiLooper2 v0.2");

  // memset(testBuffer, 0, sizeof(testBuffer));
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

void updateMidiLooper(MidiLooper &midiLooper, MultiTouchPad &touchPad, int& currentMode, Max7219Matrix& ledMatrix)
{
  const int LearnModePad = 12;
  const int RecordingModePad = 13;
  const int PlayModePad = 14;

  // 8 tracks + metronome
  const int TrackPads[] = {0,1,2,3, 4,5,6,7, 8,9,10,11,   16,17,18,19, 20,21,22,23, 24,25,26,27};
  const int MetronomePad = 15;
  const int TrackLedY[] = {0,1,2,3, 0,1,2,3, 0,1,2,3,   4,5,6,7, 4,5,6,7, 4,5,6,7 };
  const int TrackLedX[] = {5,5,5,5, 6,6,6,6, 7,7,7,7,   5,5,5,5, 6,6,6,6, 7,7,7,7 };
  const int MetronomeLedY = 7;
  const int MetronomeLedX = 4;
  
  touchPad.Read();//TODO devBoard.update()
  unsigned long currMillis = millis();
  learnModePadState.update(currMillis, touchPad.Get(LearnModePad));
  recordingModePadState.update(currMillis, touchPad.Get(RecordingModePad));
  playModePadState.update(currMillis, touchPad.Get(PlayModePad));


  // toggle num bars
  if(playModePadState.Tapped())
  {
    midiLooper.m_Ticker.ToggleNumBars(1, 3);
  }

  // determine mode
  if (touchPad.Get(LearnModePad))
  {
    currentMode = LearnMode;
  }
  else if (touchPad.Get(RecordingModePad))
  {
    currentMode = RecordingMode;
  }
  else//if (touchPad.IsClicked(PlayModePad))
  {
     currentMode = PlayMode;
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
    else if(currentMode == PlayMode)
    {
      if(midiLooper.m_Metronome.IsLearning())
      {
        midiLooper.m_Metronome.ToggleMidiLearn();
      }
      else
      {
        midiLooper.m_Metronome.onToggleMuted();
      }    
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
          if(midiLooper.m_Track[idx].m_MidiLearn)
          {
            midiLooper.m_Track[idx].StopMidiLearn();
          }
          else if(midiLooper.m_Track[idx].m_Recording)
          {
            midiLooper.m_Track[idx].ToggleRecording();
          }
          else
          {
            midiLooper.m_Track[idx].onToggleMuted(midiLooper.m_MidiOut);
          }
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

  bool fastBlinkOn = (currMillis>>7) & 0x01;
  bool slowBlinkOn = (currMillis>>8) & 0x01;
  for (int idx = 0; idx < MidiLooper::NumTracks; ++idx)
  {
          
      // recording leds: blink if midi learn, on/off ~recording otherwise
      if(midiLooper.m_Track[idx].m_MidiLearn)
      { 
        if(fastBlinkOn)
        {
            ledMatrix.Set(TrackLedX[idx], TrackLedY[idx]);
        }
        else
        {
          ledMatrix.Clear(TrackLedX[idx], TrackLedY[idx]);
        }      
      }
      else if(midiLooper.m_Track[idx].m_Recording)
      {
        if(slowBlinkOn)
        {
            ledMatrix.Set(TrackLedX[idx], TrackLedY[idx]);
        }
        else
        {
          ledMatrix.Clear(TrackLedX[idx], TrackLedY[idx]);
        }       }
      else
      {
        // play leds on/off ~play/mute
        if(midiLooper.m_Track[idx].m_Muted)
        {
          ledMatrix.Clear(TrackLedX[idx], TrackLedY[idx]);
        }
        else
        {
          ledMatrix.Set(TrackLedX[idx], TrackLedY[idx]);        
        }
      }
  }

  // metronome
  if(midiLooper.m_Metronome.IsLearning())
  {
    if(fastBlinkOn)
    {
      ledMatrix.Set(MetronomeLedX, MetronomeLedY);
    }
    else
    {
      ledMatrix.Clear(MetronomeLedX, MetronomeLedY);
    }
  }
  // no learning
  else
  {
    if(midiLooper.m_Metronome.IsPlaying())
    {
      ledMatrix.Set(MetronomeLedX, MetronomeLedY);
    }
    else
    {
      ledMatrix.Clear(MetronomeLedX, MetronomeLedY);        
    } 
  }

  ledMatrix.WriteAll();
}

void loop()
{
  // put your main code here, to run repeatedly:
  devBoard.serialDebug.println("starting up!");

  ScanI2C(devBoard.serialDebug);

  
  // startup checks
  devBoard.update();
  if(devBoard.debugActive())
  {
    devBoard.serialDebug.println("debug startup checks!");
    
    ScanI2C(devBoard.serialDebug);

    devBoard.serialDebug.println("test led matrix");
    testDigitalOutMatrix(devBoard.ledMatrix, 1);

    devBoard.serialDebug.println("test touchpad");
    const int numRepeats = 16;
    TestTouchPad(devBoard.touchPad, devBoard.serialDebug, numRepeats);
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

    updateMidiLooper(midiLooper, devBoard.touchPad, currentMode, devBoard.ledMatrix);

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



void not_loop() {
  // put your main code here, to run repeatedly:

  ScanI2C(devBoard.serialDebug);
  
  const int waitMilliSeconds = 200; 

  devBoard.LedOn();  
  devBoard.serialDebug.println("LED is on");

  testDigitalOutMatrix(devBoard.ledMatrix, 1);

  for(int repeat = 0; repeat<waitMilliSeconds; ++repeat)
  {
    devBoard.update();
     delay(1);
  }
  PrintTouchPad(devBoard.MPR121A, devBoard.serialDebug);

  devBoard.LedOff();
  Serial.println("LED is off");

  testDigitalOutMatrix(devBoard.ledMatrix, 1);

  for(int repeat = 0; repeat<waitMilliSeconds; ++repeat)
  {
     devBoard.update();
     delay(1);
   }
  PrintTouchPad(devBoard.MPR121A, devBoard.serialDebug);

}
