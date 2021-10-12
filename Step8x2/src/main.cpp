#include <Arduino.h>
#include <SPI.h>

#include "analoginbank.h"
#include "digitaloutbank.h"

//TODO chaining of 8 + 8 = 16?
// button/pot controls state -> state struct
//    => debug print state
// (??) TODO (??) hold input => no step advance

struct Step8x2State
{
  static const int NumSteps = 8;
  
  int stepA;
  int stepB;

  uint16_t gateDivider;
  
  uint16_t stepDividerA;
  uint16_t stepLengthA;
  
  uint16_t stepDividerB;
  uint16_t stepLengthB;
  
  bool chaining;

  Step8x2State()
   : stepA(0)
   , stepB(0)
   , gateDivider(1)
   , stepDividerA(1)
   , stepLengthA(NumSteps)
   , stepDividerB(1)
   , stepLengthB(NumSteps)
   , chaining(false)
  {
    
  }

};

void PrintState(Step8x2State& state)
{
  Serial.print("A / ");
  Serial.print(state.stepDividerA);
  Serial.print(" L ");
  Serial.print(state.stepLengthA);
  Serial.print(" S ");
  Serial.println(state.stepA);

  Serial.print("B / ");
  Serial.print(state.stepDividerB);
  Serial.print(" L ");
  Serial.print(state.stepLengthB);
  Serial.print(" S ");
  Serial.println(state.stepB);

  Serial.print("G / ");
  Serial.print(state.gateDivider);

  Serial.print("Ch : ");
  Serial.println(state.chaining);

}


struct Step8x2App
{
  static const int clockInPin = 2;
  static const int resetInPin = 3;

  static const int gateOutPin = 4;
  static const int triggerOutPin = 11;
  
  // analog in bank for controls -> read 1 at a time 
  static const int gateDividerInPin = A3;
  static const int stepDividerInPin1 = A4;
  static const int stepDividerInPin2 = A5;
  static const int stepLengthInPin1 = A6;
  static const int stepLengthInPin2 = A7;


  uint16_t m_StepCounter;
  uint8_t m_GateCounter;
  uint8_t m_ClockHistory;
  uint8_t m_ResetHistory;

  uint16_t m_DebugCounter;
  uint32_t m_DebugMillis;

  //TODO DigitalInBank controls -> button debouncer???
  DigitalOutBank digitalOutSelect; // ABC CV row 1, ABC CV row 2, A/B output 1, A/B output 2 
  //DigitalOutBank digitalOutGate;// gate out, trigger out, 
  AnalogInBank analogInBankControls;
 
  Step8x2State state;

  Step8x2App() 
  : m_StepCounter(0)
  , m_GateCounter(0)
  , analogInBankControls(gateDividerInPin, stepDividerInPin1, stepDividerInPin2, stepLengthInPin1, stepLengthInPin2)
  , state()
  {}

  void Begin()
  {
    m_DebugCounter = 0;

    m_StepCounter= 0;
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
    
    if((m_ResetHistory & 0x03) == 1)
    {
      //reset rising
      m_StepCounter = 0;
      m_GateCounter = 0;
    }
    else if((m_ClockHistory & 0x03) == 0x01)
    {
      //rising
      if(m_GateCounter & 0x01)
      {
        ++m_GateCounter;
      }
      ++m_StepCounter;
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
    // if pot at max => chaining
    state.chaining = (1016<state.stepLengthA);
    state.stepLengthA = 1 + (state.stepLengthA>>7);//[1,8]
      
    state.stepLengthB = analogInBankControls.get(4);
    state.stepLengthB = 1 + (state.stepLengthB>>7);//[1,8]


    // 1) shift out select ABC
    // 4) Gate out
    {
        uint16_t stepA = m_StepCounter / state.stepDividerA;
        stepA = stepA % state.stepLengthA;
        state.stepA = stepA;

        digitalOutSelect.set(0, stepA & 1);
        stepA = stepA >> 1;
        digitalOutSelect.set(1, stepA & 1);
        stepA = stepA >> 1;
        digitalOutSelect.set(2, stepA & 1);
    }
    {
        uint16_t stepB = m_StepCounter / state.stepDividerB;
        stepB = stepB % state.stepLengthB;
        state.stepB = stepB;

        digitalOutSelect.set(3, stepB & 1);
        stepB = stepB >> 1;
        digitalOutSelect.set(4, stepB & 1);
        stepB = stepB >> 1;
        digitalOutSelect.set(5, stepB & 1);
    }
    digitalOutSelect.update(0);

    // 4) gate out
    uint8_t dividedGateCounter = m_GateCounter / state.gateDivider;
    digitalWrite(gateOutPin, 1-(dividedGateCounter & 1));
    //TODO trigger: fixed length TODO TriggerOut class    
  }

  void PrintDebug(uint16_t period)
  {
    if(m_DebugCounter >= period)
    {
      uint32_t newMillis = millis();

      Serial.print(m_StepCounter);
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