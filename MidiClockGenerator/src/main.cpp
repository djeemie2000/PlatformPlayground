#include <Arduino.h>

//TODO POC 
// button -> toggle running
// led buitlin on/off ~ running
// send serial midi start / clock / stop at some fixed interval e.g. 20 msec 
// -> 20 msec x 24 PPG = 480 msec approx 2 beats per second approx 120BPM 

// TODO pot-< clock speed
// TODO digital outs ~ ClockOutState (counter, period,on Tick() Reset() )
//TODO fast pinwrite / read !!

// step 2 serial.write/outputs in interrupt
// TODO timerinterrupt library

int clockPeriod24PPQ;
int loopCounter;
int prevRunning;

void setup() {
  // put your setup code here, to run once:
  // serial port -> midi -> 31250 baudrate
  Serial.begin(31250);

  pinMode(A0, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  // -> 20 msec x 24 PPQ = 480 msec 
  // approx 2 beats per second 
  // approx 120BPM 
  clockPeriod24PPQ = 20;
  
  loopCounter = 0;
  prevRunning = 0;

  //flash led??

  for(int repeat = 0; repeat<4; ++repeat)
  {
    digitalWrite(LED_BUILTIN, 1);
    delay(200);
    digitalWrite(LED_BUILTIN, 0);
    delay(200);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  int running = digitalRead(A0);
  //TODO debounce 
  if(running)
  {
    digitalWrite(LED_BUILTIN, 1);
  }
  else
  {
    digitalWrite(LED_BUILTIN, 0);
  }

  // clock period: range 10 msec ~ 240 BPM / 40 msec ~ 60 BPM , default 20 ~ 120BPM
  int speedPot = analogRead(A1);
  clockPeriod24PPQ = map(speedPot, 0, 1023, 40, 10);

  if(!prevRunning && running)
  {
    Serial.write(0xFA);//midi start
    // send clock ??
    loopCounter = 0; // reset
  }
  else if(prevRunning && !running)
  {
    Serial.write(0xFC);//midi stop
  }
  else if(running && loopCounter == 0)
  {
    Serial.write(0xF8);//midiclock
  }

  prevRunning = running;

  //20 msec fixed delay
  ++loopCounter;
  if(clockPeriod24PPQ<loopCounter)
  {
    loopCounter = 0;
  }

  delay(1);//resolution & msec
}
