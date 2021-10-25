#include <Arduino.h>
#include <SPI.h>

#include "analoginbank.h"
#include "digitaloutbank.h"
#include "CVClockState.h"

//TODO chaining of 8 + 8 = 16?
// button/pot controls state -> state struct
//    => debug print state
// (??) TODO (??) hold input => no step advance
// (??) TODO chaining out led??

struct Step8x2State
{
  static const int NumSteps = 8;
  
  uint16_t stepA;
  uint16_t stepB;

  uint16_t gateDivider;
  uint16_t gateDuration;
  
  uint16_t stepDividerA;
  uint16_t stepLengthA;
  int enableA;
  
  uint16_t stepDividerB;
  uint16_t stepLengthB;
  int enableB;
  
  bool chaining;

  Step8x2State()
   : stepA(0)
   , stepB(0)
   , gateDivider(1)
   , gateDuration(256)
   , stepDividerA(1)
   , stepLengthA(NumSteps)
   , enableA(1)
   , stepDividerB(1)
   , stepLengthB(NumSteps)
   , enableB(1)
   , chaining(false)
  {
    
  }

  void AdvanceA()
  {
    ++stepA;
    if(stepLengthA<=stepA)
    {
      stepA = 0;
    }
  }

  void AdvanceB()
  {
    ++stepB;
    if(stepLengthB<=stepB)
    {
      stepB = 0;
    }

  }
};

void PrintState(Step8x2State& state)
{
  Serial.print("A / ");
  Serial.print(state.stepDividerA);
  Serial.print(" L ");
  Serial.print(state.stepLengthA);
  Serial.print(" S ");
  Serial.print(state.stepA);
  Serial.print(" E ");
  Serial.println(state.enableA);

  Serial.print("B / ");
  Serial.print(state.stepDividerB);
  Serial.print(" L ");
  Serial.print(state.stepLengthB);
  Serial.print(" S ");
  Serial.print(state.stepB);
  Serial.print(" E ");
  Serial.println(state.enableB);

  Serial.print("G / ");
  Serial.println(state.gateDivider);

  Serial.print("A->B : ");
  Serial.println(state.chaining);
}


struct Step8x2App
{
  static const int clockInPin = 2;
  static const int resetInPin = 3;

  static const int gateOutPin = 4;
  static const int triggerOutPin = 11;//CVClock gate out
  
  // analog in bank for controls -> read 1 at a time 
  static const int gateDividerInPin = A3;
  static const int stepDividerInPin1 = A4;
  static const int stepDividerInPin2 = A5;
  static const int stepLengthInPin1 = A6;
  static const int stepLengthInPin2 = A7;
  static const int gateDurationPin = A0;


  uint16_t m_StepCounterA;
  uint16_t m_StepCounterB;
  uint8_t m_GateCounter;
  uint8_t m_ClockHistory;
  uint8_t m_ResetHistory;

  uint16_t m_DebugCounter;
  uint32_t m_DebugMillis;

  //TODO DigitalInBank controls -> button debouncer???
  DigitalOutBank digitalOutSelect; // ABC CV row 1, ABC CV row 2, A/B output 1, A/B output 2 
  //DigitalOutBank digitalOutGate;// gate out, trigger out, 
  AnalogInBank analogInBankControls;

  CVClockState cvClockState;
 
  Step8x2State state;

  Step8x2App() 
  : m_StepCounterA(0)
  , m_StepCounterB(0)
  , m_GateCounter(0)
  , analogInBankControls(gateDividerInPin, stepDividerInPin1, stepDividerInPin2, stepLengthInPin1, stepLengthInPin2, gateDurationPin)
  , state()
  {}

  void Begin()
  {
    m_DebugCounter = 0;

    m_StepCounterA = 0;
    m_StepCounterB = 0;
    m_GateCounter = 0;
    m_ClockHistory = 0;
    m_ResetHistory = 0;

    pinMode(clockInPin, INPUT_PULLUP);
    pinMode(resetInPin, INPUT_PULLUP);

    pinMode(gateOutPin, OUTPUT);
    pinMode(triggerOutPin, OUTPUT);

    analogInBankControls.begin();
    digitalOutSelect.begin(5,6,7, 8,9,10, A1,A2);
  }

  void Update()
  {
    ++m_DebugCounter;

    m_ClockHistory = (m_ClockHistory<<1) | digitalRead(clockInPin);
    m_ResetHistory = (m_ResetHistory<<1) | digitalRead(resetInPin);
    
    bool advance = false;
    if((m_ResetHistory & 0x03) == 1)
    {
      //reset rising
      m_StepCounterA = 0;
      m_StepCounterB = 0;
      m_GateCounter = 0;
      state.stepA = 0;
      state.stepB = 0;
    }
    else if((m_ClockHistory & 0x03) == 0x01)
    {
      //rising
      if(m_GateCounter & 0x01)
      {
        ++m_GateCounter;
      }
      if(state.enableA)
      {
        ++m_StepCounterA;
      }
      if(state.enableB)
      {
        ++m_StepCounterB;
      }
      advance = true;
    }
    else if((m_ClockHistory & 0x03) == 0x02)
    {
      //falling
      if(!(m_GateCounter & 0x01))
      {
        ++m_GateCounter;
      }
    }

    analogInBankControls.updateOne();

    const uint16_t numDividers = 10; 
//    uint16_t dividers[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
//    uint16_t dividers[] = {1, 2, 2, 4, 4, 8, 8, 16, 16, 32};
    uint16_t dividers[] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24};

    uint16_t gateDividerIdx = analogInBankControls.get(0);
    gateDividerIdx = (gateDividerIdx * numDividers) >> 10;
    state.gateDivider = dividers[gateDividerIdx];

    uint16_t stepDividerAIdx = analogInBankControls.get(1);
    stepDividerAIdx = (stepDividerAIdx * numDividers) >> 10;
    state.stepDividerA = dividers[stepDividerAIdx];

    uint16_t stepDividerBIdx = analogInBankControls.get(2);
    stepDividerBIdx = (stepDividerBIdx * numDividers) >> 10;
    state.stepDividerB = dividers[stepDividerBIdx];

    state.stepLengthA = analogInBankControls.get(3);
    state.stepLengthA = 1 + (state.stepLengthA>>7);//[1,8]
      
    state.stepLengthB = analogInBankControls.get(4);
    state.stepLengthB = 1 + (state.stepLengthB>>7);//[1,8]

    //TODO 
    // state.gateDuration = analogInBankControls.get(5);
    // if(state.gateDuration<16)
    // {
    //   state.gateDuration = 0;
    // }
    // else if(1008<state.gateDuration)
    // {
    //   state.gateDuration = 1024;
    // }

    // if pot at max => chaining
    state.chaining = (1016<analogInBankControls.get(3));

    // 1) shift out select ABC
    // 4) Gate out
    if(advance)
    {
      bool advanceA = (m_StepCounterA % state.stepDividerA) == 0;
      bool advanceB = (m_StepCounterB % state.stepDividerB) == 0;

      if(state.chaining)
      {
        if(state.enableA && advanceA)
        {
            bool endOfSequence = state.stepA +1 >= state.stepLengthA;
            if(endOfSequence)
            {
                state.enableA = 0;
                state.enableB = 1;
            }
        }
        else if(state.enableB && advanceB)
        {
           bool endOfSequence = state.stepB +1 >= state.stepLengthB;
            if(endOfSequence)
            {
              state.enableA = 1;
              state.enableB = 0;
            }
        }
      }
      else
      {
        //not chaining
        if(state.enableA && advanceA)
        {
            bool endOfSequence = state.stepA +1 >= state.stepLengthA;
            if(endOfSequence)
            {
                state.enableA = 1;
                state.enableB = 1;
            }
        }
        if(state.enableB && advanceB)
        {
           bool endOfSequence = state.stepB +1 >= state.stepLengthB;
            if(endOfSequence)
            {
              state.enableA = 1;
              state.enableB = 1;
            }
        }
      }

      if(state.enableA && advanceA)
      {
        state.AdvanceA();//state.stepA = stepA;

        uint16_t stepA = state.stepA;
        digitalOutSelect.set(0, stepA & 1);
        stepA = stepA >> 1;
        digitalOutSelect.set(1, stepA & 1);
        stepA = stepA >> 1;
        digitalOutSelect.set(2, stepA & 1);
      }

      if(state.enableB && advanceB)
      {
        state.AdvanceB();//state.stepB = stepB;

        uint16_t stepB = state.stepB;
        digitalOutSelect.set(3, stepB & 1);
        stepB = stepB >> 1;
        digitalOutSelect.set(4, stepB & 1);
        stepB = stepB >> 1;
        digitalOutSelect.set(5, stepB & 1);
      }    
      

      
    }

    // 0 <-> A, 1 <-> B
    int selectAB1 = state.enableA ? 0 : 1; 
    int selectAB2 = state.enableB ? 1 : 0; 
    digitalOutSelect.set(6, selectAB1);
    digitalOutSelect.set(7, selectAB2); 
     

    digitalOutSelect.update(0);

    // 4) gate out
    uint8_t dividedGateCounter = m_GateCounter / state.gateDivider;
    digitalWrite(gateOutPin, 1-(dividedGateCounter & 1));

    int cvGate = cvClockState.Tick( 1-(dividedGateCounter & 1), state.gateDuration);
    digitalWrite(triggerOutPin, cvGate);
  }

  void PrintDebug(uint16_t period)
  {
    if(m_DebugCounter >= period)
    {
      uint32_t newMillis = millis();

      Serial.print(m_StepCounterA);
      Serial.print(" ");
      Serial.print(m_StepCounterB);
      Serial.print(" ");
      Serial.print(m_GateCounter);
      Serial.print(" ");
    //  Serial.print(m_ClockHistory, BIN);
    //  Serial.print(" ");
      Serial.print(m_DebugCounter);
      Serial.print(" ");
      Serial.println(newMillis - m_DebugMillis);
      
      PrintState(state);

      m_DebugCounter = 0;
      m_DebugMillis = newMillis;
    }

  }
};

Step8x2App app;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Step8x2 v0.3");

  app.Begin();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  app.Update();

  app.PrintDebug(1000);

  delay(1);//??needed??
}