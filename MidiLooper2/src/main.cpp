#include <Arduino.h>
#include <Wire.h>
#include "ScanI2C.h"
#include "MPR121TouchPad.h"
#include "TestTouchPad.h"
#include "Max7219Matrix.h"
#include "TestDigitalOutMatrix.h"
#include "MidiParser.h"
#include "MidiLooper.h"
#include "StopWatch.h"
#include "DigitalOutBank.h"
#include "ButtonState.h"
#include "DigitalIn.h"
#include "DigitalInBank.h"

#define FAKECLOCK 1

//uint8_t testBuffer[16*1024];
MidiParser midiParser;
MidiLooper midiLooper;

struct DevBoard
{
  static const int LedPin = 2;
  static const int csPinLedMatrix = 5;
  static const int irqPinTouchPad = 15;
  static const int debugPin = 4;// TODO debugIn DIn


  // uart0 : usb + GPIO 1 & 3
  // uart1 : not connected
  // uart2 : GPIO 16 & 17
  HardwareSerial serialDebug;//uart 0
  bool debugIsActive;

  HardwareSerial serialMidi;// uart 2
  // TODO membank
  DigitalInBank IOXP1;
//  DigitalOutBank IOXP2;
//  DigitalOutBank IOXP3;
//  AnalogIn Pot1;
//  AnalogIn Pot2;
//  AnalogIn Pot3;
  MPR121TouchPad MPR121A;
  Max7219Matrix ledMatrix;//(1,PA4);//cs pin

  DevBoard()
   : serialDebug(0)
   , debugIsActive(false)
   , serialMidi(2)
   , IOXP1(34, 35)
  //  , IOXP2(PB3, PA15, PA12, PA11)
  //  , IOXP3(PB7, PB6, PB5, PB4)
  //  , Pot1()
  //  , Pot2()
  //  , Pot3()
   , MPR121A()
   , ledMatrix(1, csPinLedMatrix)//cs pin
   {}

   void Begin()
   {
     pinMode(LedPin, OUTPUT);// led builtin
     pinMode(debugPin, INPUT_PULLUP);

     serialDebug.begin(115200);
     serialMidi.begin(31250);

     IOXP1.begin();
    //  IOXP2.begin();
    //  IOXP3.begin();

    //  Pot1.begin(PA0);
    //  Pot2.begin(PA1);
    //  Pot3.begin(PA2);

     // setup I2C for MPR121 MPR121A
     // Wire.begin() is called inside MPR121A.begin()
     MPR121A.Begin(irqPinTouchPad); //irq pin
     //TODO autoDetect I2C : MPR121A memBank

      //SPI CS external
      ledMatrix.Configure();     
   }

   void update()
   {
     //TODO!!!!!!!!!!!!!!
    debugIsActive = true;//TODO!!!(0 == digitalRead(debugPin));
    IOXP1.update();
    // IOXP2.update();
    // IOXP3.update();

    //  Pot1.Read();
    //  Pot2.Read();
    //  Pot3.Read();
    // update led matrix, touchpad here??
    MPR121A.Read();
    ledMatrix.WriteAll();
   }

   //TODO update() -> store in variable
   bool debugActive() const
   {
     return debugIsActive; 
   }

   void LedOn()
   {
     digitalWrite(LedPin, HIGH);
   }

   void LedOff()
   {
     digitalWrite(LedPin, LOW);
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


void setup() {
  // put your setup code here, to run once:
  devBoard.Begin();

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

void updateMidiLooper(MidiLooper &midiLooper, MPR121TouchPad &touchPad, int& currentMode, Max7219Matrix& ledMatrix)
{
  const int LearnModePad = 0;
  const int RecordingModePad = 3;
  const int PlayModePad = 6;

  const int MetronomePad = 9;
  const int TrackPads[] = {1,2, 4,5, 7,8, 10,11};
  // 8 tracks + metronome
  const int playY[] = {6,7, 6,7, 6,7, 6,7, 5};
  const int playX[] = {4,4, 5,5, 6,6, 7,7, 7};
  
  touchPad.Read();
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
            ledMatrix.Set(playX[idx], playY[idx]);
        }
        else
        {
          ledMatrix.Clear(playX[idx], playY[idx]);
        }      
      }
      else if(midiLooper.m_Track[idx].m_Recording)
      {
        if(slowBlinkOn)
        {
            ledMatrix.Set(playX[idx], playY[idx]);
        }
        else
        {
          ledMatrix.Clear(playX[idx], playY[idx]);
        }       }
      else
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
      }
  }

  // metronome
  //   play led ~~ playing 
  //   recording led blink if midi learn
  if(midiLooper.m_Metronome.IsLearning())
  {
    if(fastBlinkOn)
    {
      ledMatrix.Set(playX[MidiLooper::NumTracks], playY[MidiLooper::NumTracks]);
    }
    else
    {
      ledMatrix.Clear(playX[MidiLooper::NumTracks], playY[MidiLooper::NumTracks]);
    }
  }
  // no recording
  else
  {
    if(midiLooper.m_Metronome.IsPlaying())
    {
      ledMatrix.Set(playX[MidiLooper::NumTracks], playY[MidiLooper::NumTracks]);
    }
    else
    {
      ledMatrix.Clear(playX[MidiLooper::NumTracks], playY[MidiLooper::NumTracks]);        
    } 
  }

  ledMatrix.WriteAll();
}

void loop()
{
  // put your main code here, to run repeatedly:
  devBoard.serialDebug.println("starting up!");


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
    TestTouchPad(devBoard.MPR121A, devBoard.serialDebug, numRepeats);
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
