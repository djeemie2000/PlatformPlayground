#include <Arduino.h>
#include "fastdigitalwrite.h"
#include "clockoutstate.h"

#define USE_TIMER_1     true
#define USE_TIMER_2     false
#define USE_TIMER_3     false

#include "TimerInterrupt.h"

// send serial midi start / clock / stop at some interval e.g. 20 msec 
// -> 20 msec x 24 PPQ = 480 msec approx 2 beats per second approx 120BPM 

// button -> toggle running
// pot -> clock speed
// led buitlin on/off ~ running
// digital outs 
//  1/16 clock 
//  tempo led (TBD?)
//  reset pulse upon midi start

// step 2 serial.write/outputs in interrupt
// TODO timerinterrupt library


// shared state between loop and interrupt
int clockPeriod24PPQ;
int running;

// interrupt state
int interruptCounter;
int prevRunning;
ClockOutState<int> clockOutStateSixteenthNotes;
ClockOutState<int> ledOutStateTempoIndicator;



void oninterrupt()
{
  if(!prevRunning && running)
  {
    // send first clock as well ??
    Serial.write(0xFA);//midi start
    //Serial.write(0xF8);//midiclock
    interruptCounter = 0; // reset

    clockOutStateSixteenthNotes.Reset();
    ledOutStateTempoIndicator.Reset();

    // midi devices start running upon first midi clock after midi start
  }
  else if(prevRunning && !running)
  {
    Serial.write(0xFC);//midi stop
  }
  
  if(running && interruptCounter == 0)
  {
    Serial.write(0xF8);//midiclock
    clockOutStateSixteenthNotes.Tick();
    ledOutStateTempoIndicator.Tick();
  }

  int clockValue = running ? clockOutStateSixteenthNotes.Get() : 0;
  fastDigitalWritePortC<2>(clockValue);

  int tempoLedValue = running ? ledOutStateTempoIndicator.Get() : 0;
  fastDigitalWritePortC<4>(tempoLedValue);

  // led on if running, off if not
  fastDigitalWritePortB<5>(running);

  prevRunning = running;

  ++interruptCounter;
  if(clockPeriod24PPQ<interruptCounter)
  {
    interruptCounter = 0;
  }
}

int testonoff;

void oninterrupttest()
{
  fastDigitalWritePortC<3>(testonoff);
  testonoff = 1-testonoff;//toggle 0/1
}

void setup() 
{
  // put your setup code here, to run once:

  // serial port -> midi -> 31250 baudrate
  Serial.begin(31250);

  // digital in/out pins:
  pinMode(A0, INPUT_PULLUP);
  // A1 pot analog read input
  pinMode(A2, OUTPUT); // 16th note clock output
  pinMode(A3, OUTPUT); // reset pulse output
  pinMode(A4, OUTPUT); // tempo led output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // -> 20 msec x 24 PPQ = 480 msec 
  // approx 2 beats per second 
  // approx 120BPM 
  clockPeriod24PPQ = 20;
  running = 0;

  interruptCounter = 0;
  prevRunning = 0;
  // at 24 PPQ, period = 6 => 4 PPQ => quarter note / 4  = sixteenth notes clock
  clockOutStateSixteenthNotes.Configure(3,6);

  // quarter note led blink
  ledOutStateTempoIndicator.Configure(12,24);
  
  //TODO upon midi start, singleshot reset pulse  length = one 16th note clock pulse

  // start timer with period 1 msec
  testonoff = 0;
  ITimer1.init();
  ITimer1.attachInterruptInterval(1, oninterrupt); 

  //flash led upon start??
  for(int repeat = 0; repeat<4; ++repeat)
  {
    fastDigitalWritePortB<5>(1);
    delay(200);
    fastDigitalWritePortB<5>(0);
    delay(200);
  }
}

void loop() 
{
  // put your main code here, to run repeatedly:

  running = digitalRead(A0);
  //TODO debounce 

  int prevClockPeriod24PPQ = clockPeriod24PPQ;
  // clock period: range 10 msec ~ 240 BPM / 60 msec ~ 40 BPM , default 20 ~ 120BPM
  int speedPot = analogRead(A1);
  int currentClockPeriod24PPQ = map(speedPot, 0, 1023, 60, 10);
  // smooth
  int newClockPeriod24PPQ = (3*prevClockPeriod24PPQ + currentClockPeriod24PPQ) / 4;
  //Assign to volatile
  clockPeriod24PPQ = newClockPeriod24PPQ;

//  oninterrupt();

  delay(1);//resolution 1 msec
}
