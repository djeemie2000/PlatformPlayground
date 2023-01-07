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
  hiresTicker.SetPeriod(200);// when tick every 100 uSec => 20 mSec period for 24PPQ => 480 mSec per beat ~120BPM

  timer = timerBegin(0, 80, true);// period 1 uSec (80 MHz / 80 = 1MHz)
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100, true);// period 100 uSec
  timerAlarmEnable(timer);//start timer
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
  app.ReadMidiIn(devBoard.serialMidi);
}

void loop() {
  // put your main code here, to run repeatedly:

  devBoard.serialDebug.println("loop");
  testDigitalOutMatrixRows(devBoard.ledMatrix, 1);

  int cntr = 0;
  while(cntr++<100000)
  {
    loopHiFreq();

    app.DisplayTicker(devBoard.ledMatrix);

    delay(1);
  }

  return;

  loopHiFreq();
  // the following can be handled at a slower pace
  app.ProcessMidiIn(3, devBoard.serialMidi);
  // TODO handling midi input for midi touchpad should be split into smaller parts?

  loopHiFreq();
  // TODO read buttons and handle, can be split per track?

  //TODO update ledMatrix to state -> can be split? ticker state? tracks state? 
  // -> probably not usefull to split since most of the time goes into (SPI) update of the led matrix?
  // TODO split write row per row!
  // TODO optimize led matrix code for single matrix, without rotation?
  loopHiFreq();
  app.DisplayTicker(devBoard.ledMatrix);

  loopHiFreq();
  app.DiplayTrackState(devBoard.ledMatrix);
  //---
}
