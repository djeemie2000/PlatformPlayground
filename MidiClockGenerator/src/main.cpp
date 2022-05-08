#include <Arduino.h>

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

template<int N>
void fastDigitalWritePortB(int value)
{
  if (!value) {
		PORTB &= ~(1<<N);
	} else {
		PORTB |= (1<<N);
	}
}

template<int N>
void fastDigitalWritePortC(int value)
{
  if (!value) {
		PORTC &= ~(1<<N);
	} else {
		PORTC |= (1<<N);
	}
}

//C0 = analog pin 0
//C1 = analog pin 1
//C2 = analog pin 2
//C3 = analog pin 3
//C4 = analog pin 4
//C5 = analog pin 5

// led / pin 13 = PB5


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
    fastDigitalWritePortB<5>(1);
    delay(200);
    fastDigitalWritePortB<5>(0);
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
  fastDigitalWritePortC<2>(clockValue);

  int tempoLedValue = running ? ledOutStateTempoIndicator.Get() : 0;
  fastDigitalWritePortC<3>(tempoLedValue);

  // led on if running, off if not
  fastDigitalWritePortB<5>(running);

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

  delay(1);//resolution 1 msec
}
