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
//TODO fast write portC

template<class T>
class ClockOutState
{
public:
  ClockOutState()
   : m_State(0)
   , m_Counter(0)
   , m_OnTicks(1)
   , m_Period(2)
   {}

  void Configure(int onTicks, int period)
  {
    m_OnTicks = onTicks;
    m_Period = period;
  }

   void Reset()
   {
     m_Counter = 0;
     m_State = (m_Counter<m_OnTicks) ? 1 : 0;
   }

   void Tick()
   {
     m_State = (m_Counter<m_OnTicks) ? 1 : 0;

     ++m_Counter;
     if(m_Period<=m_Counter)
     {
       m_Counter = 0;
     }
   }

  int Get() const
  {
     return m_State;
  }

private:

  int m_State;
  T m_Counter;
  T m_OnTicks;
  T m_Period;
};

//loop state
int clockPeriod24PPQ;
int running;

//oninterrupt state
int loopCounter;
int prevRunning;
ClockOutState<int> clockOutStateSixteenthNotes;
ClockOutState<int> ledOutStateTempoIndicator;

void setup() {
  // put your setup code here, to run once:
  // serial port -> midi -> 31250 baudrate
  Serial.begin(31250);

  pinMode(A0, INPUT_PULLUP);
  // A1 pot analog read input
  pinMode(A2, OUTPUT); // 16th note clock output
  pinMode(A3, OUTPUT); // tempo led output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // -> 20 msec x 24 PPQ = 480 msec 
  // approx 2 beats per second 
  // approx 120BPM 
  clockPeriod24PPQ = 20;
  running = 0;

  loopCounter = 0;
  prevRunning = 0;
  // at 24 PPQ, period = 6 => 4 PPQ => quarter note / 4  = sixteenth notes clock
  clockOutStateSixteenthNotes.Configure(3,6);

  // at beginning of every quarter note, short blink
  ledOutStateTempoIndicator.Configure(3,24);
  
  //TODO upon midi start, singleshot reset pulse  length = one 16th note clock pulse

  //flash led upon start??
  for(int repeat = 0; repeat<4; ++repeat)
  {
    digitalWrite(LED_BUILTIN, 1);
    delay(200);
    digitalWrite(LED_BUILTIN, 0);
    delay(200);
  }
}

void oninterrupt()
{
  if(!prevRunning && running)
  {
    // send first clock as well ??
    Serial.write(0xFA);//midi start
    //Serial.write(0xF8);//midiclock
    loopCounter = 0; // reset

    clockOutStateSixteenthNotes.Reset();
    ledOutStateTempoIndicator.Reset();

    // midi devices start running upon first midi clock after midi start
  }
  else if(prevRunning && !running)
  {
    Serial.write(0xFC);//midi stop
    //clock out off
    digitalWrite(A2, 0);
  }
  
  if(running && loopCounter == 0)
  {
    Serial.write(0xF8);//midiclock
    clockOutStateSixteenthNotes.Tick();
    ledOutStateTempoIndicator.Tick();
  }

  int clockValue = running ? clockOutStateSixteenthNotes.Get() : 0;
  digitalWrite(A2, clockValue);

  int tempoLedValue = running ? ledOutStateTempoIndicator.Get() : 0;
  digitalWrite(A3, tempoLedValue);

  if(running)
  {
    digitalWrite(LED_BUILTIN, 1);
  }
  else
  {
    digitalWrite(LED_BUILTIN, 0);
  }

  prevRunning = running;

  //20 msec fixed delay
  ++loopCounter;
  if(clockPeriod24PPQ<loopCounter)
  {
    loopCounter = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  running = digitalRead(A0);
  //TODO debounce 

  // clock period: range 10 msec ~ 240 BPM / 40 msec ~ 60 BPM , default 20 ~ 120BPM
  int speedPot = analogRead(A1);
  clockPeriod24PPQ = map(speedPot, 0, 1023, 40, 10);

  oninterrupt();

  delay(1);//resolution & msec
}
