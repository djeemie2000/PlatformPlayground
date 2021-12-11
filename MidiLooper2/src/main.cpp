#include <Arduino.h>
#include <Wire.h>
#include "ScanI2C.h"
#include "TestTouchPad.h"
#include "TestDigitalOutMatrix.h"
#include "MidiParser.h"
#include "MidiLooper.h"
#include "StopWatch.h"
#include "DigitalOutBank.h"
#include "DevBoard.h"
#include "MidiLooperStorage.h"
#include "MidiLooperClock.h"

#include "Preferences.h"


//SPIClass SPI2(HSPI);

//#define FAKECLOCK 1

DevBoard devBoard;

void PrintStorage(HardwareSerial& serial, MidiLooperStorage& storage, uint8_t slot)//TODO hardwareserial , move to storage header
{


  // iterate tracks:
  // midi channel
  // play/mute
  // events?? events size??
  serial.print("Slot ");
  serial.println(slot, HEX);

  storage.Open(slot);
  storage.PrintStats(serial);

  for(int track = 0; track<MidiLooper::NumTracks; ++track)
  {
      serial.print("Track ");
      serial.print(track);
      serial.print(" Ch 0x");
      uint8_t ch = 0xFF;
      if(storage.LoadMidiChannel(track, ch))
      {
          serial.print(ch, HEX);
      }
      else
      {
          serial.print("??");
      }
      bool play = false;
      serial.print(" Pl ");
      if(storage.LoadPlayMute(track, play))
      {
          serial.print(play);
      }
      else
      {
          serial.print("??");
      }
      int numEvents = 0;
      serial.print(" Ev# ");
      if(storage.LoadNumEvents(track, numEvents))
      {
          serial.print(numEvents);
      }
      else
      {
          serial.print("??");
      }
      serial.println();      
  }
  storage.Close();
}


struct MidiLooperApp
{
  static const int SaveMode = 0;
  static const int LoadMode = 1;
  static const int LearnMode = 4;
  static const int RecordingMode = 5;
  static const int EraseLayerMode = 6;
  static const int PlayMode = 7;
  int currentMode;

  uint8_t currentSlot;

  MidiLooperClock looperClock;
  MidiParser midiParser;
  MidiLooper midiLooper;
  MidiLooperStorage midiLooperStorage;

  void Begin(HardwareSerial* serialMidi);
  void AllNotesOff();

  void Save(uint8_t slot);
  void Load(uint8_t slot);//TODO

  void Tick(int bpmUnmapped);
  void readMidiIn(HardwareSerial &serialMidi, int maxNumBytesRead = 3);
  void updateMidiLooper(MultiTouchPad &touchPad, Max7219Matrix& ledMatrix, TouchInBank& touchIn);
};

MidiLooperApp midiLooperApp;

void MidiLooperApp::Begin(HardwareSerial* serialMidi)
{
  currentMode = MidiLooperApp::LearnMode;
  currentSlot = 0x00;
  midiLooper.begin(&devBoard.serialMidi); //calls serialMidi.begin(31250);
}

void MidiLooperApp::Tick(int bpmUnmapped)
{
    #ifdef FAKECLOCK
    looperClock.Tick();
    #endif

    #ifndef FAKECLOCK
    looperClock.Tick(bpmUnmapped);
    #endif

    midiLooper.onTick(looperClock.Get());
}

void MidiLooperApp::AllNotesOff()
{
  for(int ch =0; ch<16; ++ch)
  {
    midiLooper.m_MidiOut.allNotesOff(ch);
  }
}

void MidiLooperApp::Save(uint8_t slot)
{
  midiLooper.Save(midiLooperStorage, slot);
}

void MidiLooperApp::Load(uint8_t slot)
{
  midiLooper.Load(midiLooperStorage, slot);
}

void setup() {
  // put your setup code here, to run once:
  devBoard.Begin();
  delay(500); // Allow 500ms for the 8229BSF to get ready after turn-on

  midiLooperApp.Begin(&devBoard.serialMidi); //calls serialMidi.begin(31250);

  delay(1000);
  devBoard.serialDebug.println("MidiLooper2 v0.4");

}

void MidiLooperApp::readMidiIn(HardwareSerial &serialMidi, int maxNumBytesRead)
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
    midiParser.Parse(byte, midiLooper);

    //devBoard.serialDebug.println(byte, HEX);

    --numBytesIn;
  }
}

void MidiLooperApp::updateMidiLooper(MultiTouchPad &touchPad, Max7219Matrix& ledMatrix, TouchInBank& touchIn)
{
  const int LearnModePad = 12;
  const int RecordingModePad = 13;
  const int EraseLayerPad = 14;

  // 8 tracks + metronome
  const int TrackPads[] = {0,1,2,3, 4,5,6,7, 8,9,10,11,   16,17,18,19, 20,21,22,23, 24,25,26,27};
  const int MetronomePad = 15;
  const int TrackLedY[] = {0,1,2,3, 0,1,2,3, 0,1,2,3,   4,5,6,7, 4,5,6,7, 4,5,6,7 };
  const int TrackLedX[] = {5,5,5,5, 6,6,6,6, 7,7,7,7,   5,5,5,5, 6,6,6,6, 7,7,7,7 };
  const int MetronomeLedY = 7;
  const int MetronomeLedX = 4;
  
  touchPad.Read();//TODO devBoard.update()
  unsigned long currMillis = millis();


  // toggle num bars
  // if(playModePadState.Tapped())
  // {
  //   midiLooper.m_Ticker.ToggleNumBars(1, 3);
  // }
  // TODO read numBars from pot and set on ticker

  touchIn.update();
  if(touchIn.IsClicked(0))
  {
    //TODO check if track is clicked, => do not check other track stuff
    Save(currentSlot);
  }
  else if(touchIn.IsClicked(1))
  {
    //TODO check if track is clicked, => do not check other track stuff
    Load(currentSlot);
  }
  else if(touchIn.IsClicked(2))
  {
    ::PrintStorage(devBoard.serialDebug, midiLooperStorage, currentSlot);
  }
  else if(touchIn.IsClicked(3))
  {
    //TODO check if track is clicked, => do not check other track stuff
    midiLooper.printState(devBoard.serialDebug);
  }

  // determine mode
  if(touchIn.get(0))
  {
    currentMode = SaveMode;
  }
  else if(touchIn.get(1))
  {
    currentMode = LoadMode;
  }
  //TODO printstorage(~slot)
  else if (touchPad.Get(LearnModePad))
  {
    currentMode = LearnMode;
  }
  else if (touchPad.Get(RecordingModePad))
  {
    currentMode = RecordingMode;
  }
  else if(touchPad.Get(EraseLayerPad))
  {
    currentMode = EraseLayerMode;
  }
  else
  {
     currentMode = PlayMode;
  }

  // metronome update
  if (touchPad.IsClicked(MetronomePad))
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
    if(touchPad.IsClicked(TrackPads[idx]))//trackPadState[idx].Tapped())
    {
        // serialDebug.print("track pad ");
        // serialDebug.print(idx);
        // serialDebug.println(" tap");

        // TODO all notes off -> touchpad????
        if(currentMode == LearnMode)
        {
          midiLooper.ToggleMidiLearn(idx);
        }
        else if(currentMode == RecordingMode)
        {
          midiLooper.ToggleRecording(idx);
        }
        else if(currentMode == EraseLayerMode)
        {
          midiLooper.m_Track[idx].onUndo(midiLooper.m_MidiOut);
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
    // else if(trackPadState[idx].LongPressed())
    // {
    //     // serialDebug.print("track pad ");
    //     // serialDebug.print(idx);
    //     // serialDebug.println(" long");

    //   // long press WHEN in recording mode of that track => erase layer
    //     if(midiLooper.m_Track[idx].m_Recording)//record mode
    //     {
    //       midiLooper.m_Track[idx].onUndo(midiLooper.m_MidiOut);
    //     }
    //     else if(!midiLooper.m_Track[idx].m_MidiLearn)//play mode
    //     {
    //       midiLooper.m_Track[idx].AllNotesOff(midiLooper.m_MidiOut);
    //     }
    // }
  }

  // indicate current mode with leds (row 0)
  const int modeRow = 0;
  for(int col = 0;col<8; ++col)
  {
    ledMatrix.Clear(modeRow,col);
  }
  ledMatrix.Set(modeRow, currentMode);

  // indicate ticker state (which bar) (row 2)
  const int tickerRow = 2;
  TickerState tickerState;
  midiLooper.m_Ticker.GetTickerState(tickerState);
  for(int col = 0; col<8; ++col)
  {
    if(col<tickerState.m_NumBars)
    {
      ledMatrix.Clear(tickerRow, col);
    }
    else
    {
      ledMatrix.Set(tickerRow, col);
    }
  }
  // blink on upon first tick of each beat
  if(tickerState.m_Tick==0)
  {
    ledMatrix.Set(tickerRow, tickerState.m_Bar);
  }

  // show current slot : row 3
  const int slotRow = 3;
  for(int col = 0;col<8; ++col)
  {
    ledMatrix.Clear(slotRow,col);
  }
  ledMatrix.Set(slotRow, currentSlot);

  // tracks state : rows 5,6,7
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
        }      
      }
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

  // metronome state: row 4
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

void TestPreferences(HardwareSerial& serial)
{
  serial.println("Test preferences");
  delay(5000);

  serial.println("Testing preferences");
  Preferences preferences;

  preferences.begin("slot0", false);

  size_t free = preferences.freeEntries();
  serial.print("Free entries ");
  serial.println(free);

  uint32_t bootCount = 0;
  bootCount = preferences.getUInt("bootcount", bootCount);

  serial.print("read bootcount ");
  serial.println(bootCount);

  ++bootCount;
  size_t ok = preferences.putUInt("bootcount", bootCount);

  serial.print("write bootcount ");
  serial.print(bootCount);
  serial.print(" : ");
  serial.println(ok);

  preferences.end();

  delay(2000);
  serial.println("restart");

  delay(1000);
  ESP.restart();
}

void loop()
{
  // mode button for erase layer
  //  => no need for button state/long press/tapped! 
  //    => more responsive
  // touchin pad prev/curr state => clicked,... cfr touchpad
  // save/load ~touch in buttons
  //     => TODO how many slots??
  // TODO new layer only when no note pressed
  // TODO touchin pad (?) allnotesoff ~track 
  // print storage upon touchin pad
  // TODO (?) touchin pad => toggle start/stop cfr midi keyboard

  // put your main code here, to run repeatedly:
  devBoard.serialDebug.println("starting up!");

//  midiLooperApp.midiLooperStorage.PrintStats(devBoard.serialDebug);
  //TestPreferences(devBoard.serialDebug);
  //return;
  
  // startup checks
  devBoard.update();
  if(devBoard.debugActive())
  {
    devBoard.serialDebug.println("debug startup checks!");
    
    ScanI2C(devBoard.serialDebug);

    devBoard.serialDebug.println("test led matrix");
    testDigitalOutMatrix(devBoard.ledMatrix, 1);

    devBoard.serialDebug.println("test touch pins");
    TestTouchInBank(devBoard.touchIn, devBoard.serialDebug, 1);//16);

    devBoard.serialDebug.println("test touchpad");
    const int numRepeats = 1;// 16;
    TestTouchPad(devBoard.touchPad, devBoard.serialDebug, numRepeats);
  
    // test storage
    devBoard.serialDebug.println("print storage");
    PrintStorage(devBoard.serialDebug, midiLooperApp.midiLooperStorage, 0x00);
  }

  // make sure all notes are off on all midi channels
  devBoard.serialDebug.println("all notes off");
  midiLooperApp.AllNotesOff();

  // main loop:
  int debugCounter = 0;
  StopWatch debugTimer;
  debugTimer.start();
  
  while (true)
  {
    //fake clock
    delay(1); //TODO check if needed when using external clock input

    devBoard.Pot1.Read();
    int bpmUnmapped = devBoard.Pot1.Get();
    midiLooperApp.Tick(bpmUnmapped);
    
    // turn the LED on/off
    if(midiLooperApp.looperClock.Get())
    {
      devBoard.LedOn();
    }
    else
    { 
      devBoard.LedOff();
    }
   
    // read midi in but limit # bytes for performance issues
    midiLooperApp.readMidiIn(devBoard.serialMidi, 3);
    midiLooperApp.updateMidiLooper(devBoard.touchPad, devBoard.ledMatrix, devBoard.touchIn);

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
