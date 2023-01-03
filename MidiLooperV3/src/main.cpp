#include <Arduino.h>

#include "DevBoard.h"
#include "periodicticker.h"
#include "midilooperapp.h"

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

  // setup periodic ticker oninterrrupt
  hiresTicker.SetPeriod(400);//40 mSec period for 24PPQ => 480 mSec per beat ~120BPM

  timer = timerBegin(0, 80, true);// period 1 uSec (80 MHz / 80 = 1MHz)
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100, true);// period 100 uSec
  timerAlarmEnable(timer);//start timer
}

void loop() {
  // put your main code here, to run repeatedly:

  // ---
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
    // tick midi looper ticker (24PPQ)
    app.Tick();
    
    // send midi clock at 24PPQ
    app.PlayMidiClock(devBoard.serialMidi);
    
    // TODO detect clock on<->off
    // send midi notes upon clock off->on, upon clock on-> off
  }

  app.ReadMidiIn(devBoard.serialMidi);
  // ---


}
