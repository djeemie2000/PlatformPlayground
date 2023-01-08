#include <Arduino.h>

#include "DevBoard.h"
#include "periodicticker.h"
#include "midilooperapp.h"

#include "TestDigitalOutMatrix.h"

hw_timer_t* timer;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

DevBoard devBoard;
PeriodicTicker hiresTicker;
MidiLooperApp app;

int debugCntr;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  hiresTicker.Tick();
  portEXIT_CRITICAL_ISR(&timerMux); 
}

void setup() {
  // put your setup code here, to run once:

  devBoard.Begin();

  delay(1000);
  devBoard.serialDebug.println("MidiLooperV3 v0.1...");

  // setup periodic ticker oninterrupt
  hiresTicker.SetPeriod(300);
  // when tick every 100 uSec
  // => 20 mSec period for 24PPQ => 480 mSec per beat ~120BPM
  // 30 mSec period for 24PPQ => 720 mSec per beat ~80BPM

  timer = timerBegin(0, 80, true);// period 1 uSec (80 MHz / 80 = 1MHz)
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100, true);// period 100 uSec
  timerAlarmEnable(timer);//start timer

  app.Setup();

  debugCntr = 0;

  devBoard.ledMatrix.WriteAll();//clear all
}

void loopHiFreq()
{
  // the following needs to run as frequent as possible:
  bool tickerAdvanced = false;  
  portENTER_CRITICAL_ISR(&timerMux);
  if(hiresTicker.Peek())
  {
    hiresTicker.Clear();
    tickerAdvanced = true;
  }
  portEXIT_CRITICAL_ISR(&timerMux); 

  if(tickerAdvanced)
  {
    app.HandleTick(devBoard.serialMidi);
  }

  // regardless of ticker advanced
  app.ReadMidiIn(devBoard.serialMidi, devBoard.serialDebug);

  // experiment?
  app.ProcessMidiIn(3, devBoard.serialMidi, devBoard.serialDebug);
}

void loop() {
  // put your main code here, to run repeatedly:

  loopHiFreq();
  // the following can be handled at a slower pace
  // app.ProcessMidiIn(3, devBoard.serialMidi, devBoard.serialDebug);
  // TODO handling midi input for midi touchpad should be split into smaller parts?
  // app.ProcessMidiIn(3, devBoard.serialMidi, devBoard.serialDebug);
  

  loopHiFreq();
  // TODO read buttons and handle, can be split per track?

  //TODO update ledMatrix to state -> can be split? ticker state? tracks state? 
  // -> probably not usefull to split since most of the time goes into (SPI) update of the led matrix?
  // TODO split write row per row!
  // TODO optimize led matrix code for single matrix, without rotation?
  loopHiFreq();
  app.DisplayTicker(devBoard.ledMatrix);

  loopHiFreq();
  app.DiplayMetronomeState(devBoard.ledMatrix);

  loopHiFreq();
  app.DiplayTrackState(devBoard.ledMatrix);
  //---

  // ++debugCntr;
  // if(3000<debugCntr)
  // {
  //   devBoard.serialDebug.println(app.ticker.GetStep());
  //   devBoard.serialDebug.println(app.ticker.GetNumSteps());

  //   app.track[0].PrintItems(devBoard.serialDebug);
  //   debugCntr = 0;
  // }
}
