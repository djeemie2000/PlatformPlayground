#include <Arduino.h>
#include <SPI.h>

#include "analoginbank.h"
#include "shiftoutbank.h"
#include "analogoutbank.h"

//TODO chaining of 8 + 8 = 16?
//TODO quantize CV out
//TODO cvclock on 2nd gate ~ CV input
//    take clock divider into account!!!
// button/pot controls state + step CV -> state struct
//    => possible to store/recall settings + step CV (?) 
//    => debug print state
//TODO hold input => no step advance

struct Step8x2State
{
  static const int NumSteps = 8;
  
  int stepCVA[NumSteps];
  int stepCVB[NumSteps];
  int gateDurationCV;

  int stepA;
  int stepB;

  uint16_t gateDivider;
  
  uint16_t stepDividerA;
  uint16_t stepLengthA;
  bool quantizeA;

  uint16_t stepDividerB;
  uint16_t stepLengthB;
  bool quantizeB;

  bool chaining;

  Step8x2State()
   : gateDurationCV(512)
   , stepA(0)
   , stepB(0)
   , gateDivider(1)
   , stepDividerA(1)
   , stepLengthA(NumSteps)
   , quantizeA(false)
   , stepDividerB(1)
   , stepLengthB(NumSteps)
   , quantizeB(false)
   , chaining(false)
  {
    for(int idx = 0; idx<NumSteps;++idx)
    {
      stepCVA[idx] = 0;
      stepCVB[idx] = 0;
    }
  }

};

void PrintState(Step8x2State& state)
{
  Serial.print("A / ");
  Serial.print(state.stepDividerA);
  Serial.print(" L ");
  Serial.print(state.stepLengthA);
  Serial.print(" Q ");
  Serial.print(state.quantizeA);
  Serial.print(" S ");
  Serial.println(state.stepA);

  Serial.print("B / ");
  Serial.print(state.stepDividerB);
  Serial.print(" L ");
  Serial.print(state.stepLengthB);
  Serial.print(" Q ");
  Serial.print(state.quantizeB);
  Serial.print(" S ");
  Serial.println(state.stepB);

  Serial.print("G / ");
  Serial.print(state.gateDivider);
  Serial.print(" L ");
  Serial.println(state.gateDurationCV);

  Serial.print("Ch : ");
  Serial.println(state.chaining);

}


struct Step8x2App
{
  static const int clockInPin = 2;
  static const int resetInPin = 3;
  static const int holdInPin = 4;

  static const int chainingInPin = 5;
  static const int quantizeInPin1 = 6;
  static const int quantizeInPin2 = 7;

  static const int gateOutPin = 8;
  
  // analog in bank for controls -> read 1 at a time 
  static const int gateDividerInPin = A2;//TODO A3 etc...
  static const int stepDividerInPin1 = A3;
  static const int stepDividerInPin2 = A4;
  static const int stepLengthInPin1 = A5;
  static const int stepLengthInPin2 = A6;

  // analog in bank for CV : read all
  static const int stepCvInPin1 = A0;
  static const int stepCvInPin2 = A1;
  static const int gateCvInPin = A7;//TODO A2
  
  
  uint16_t m_StepCounter;
  uint8_t m_GateCounter;
  uint8_t m_ClockHistory;
  uint8_t m_ResetHistory;

  uint16_t m_DebugCounter;
  uint32_t m_DebugMillis;

  //TODO DigitalInBank controls -> button debouncer???
  AnalogInBank analogInBankControls;
  AnalogInBank analogInBankCVs;
  ShiftOutBank shiftOutBank;
  AnalogOutBank analogOutBankCVs;

  Step8x2State state;

  Step8x2App() 
  : m_StepCounter(0)
  , m_GateCounter(0)
  , analogInBankControls(A2, A3, A4, A5, A6)
  , analogInBankCVs(A0, A1, A7)
  , shiftOutBank(9)// analogOutBank uses CS pin 9 for first DAC!
  , analogOutBankCVs()
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
    pinMode(holdInPin, INPUT_PULLUP);

    pinMode(gateOutPin, OUTPUT);    

    analogInBankControls.begin();
    analogInBankCVs.begin();
    shiftOutBank.begin();
    analogOutBankCVs.begin(2);//uses CS pin 10 for first DAC!
  }

  void Update()
  {
    ++m_DebugCounter;

    m_ClockHistory = (m_ClockHistory<<1) | digitalRead(clockInPin);
    m_ResetHistory = (m_ResetHistory<<1) | digitalRead(resetInPin);
    //int hold = digitalRead(holdInPin);//PULLUP !!!! 
    
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
      //TODO if hold, block step counter but advance gate counter ???
      //TODO if hold, make sure that both counters still remain 'in sync' !!!!
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
    state.stepLengthA = 1 + (state.stepLengthA>>7);//[1,8]

    state.stepLengthB = analogInBankControls.get(4);
    state.stepLengthB = 1 + (state.stepLengthB>>7);//[1,8]


    // 1) shift out select ABC
    // 2) read CVs
    // 3) analogOut CVs
    // 4) Gate out
    {
        uint16_t stepA = m_StepCounter / state.stepDividerA;
//        stepA = stepA >> 1;
        stepA = stepA % state.stepLengthA;
        state.stepA = stepA;

        shiftOutBank.set(0, stepA & 1);
        stepA = stepA >> 1;
        shiftOutBank.set(1, stepA & 1);
        stepA = stepA >> 1;
        shiftOutBank.set(2, stepA & 1);
    }
    {
        uint16_t stepB = m_StepCounter / state.stepDividerB;
//        stepB = stepB >> 1;
        stepB = stepB % state.stepLengthB;
        state.stepB = stepB;

        shiftOutBank.set(4, stepB & 1);
        stepB = stepB >> 1;
        shiftOutBank.set(5, stepB & 1);
        stepB = stepB >> 1;
        shiftOutBank.set(6, stepB & 1);
    }
    shiftOutBank.update();

    // 2)
    analogInBankCVs.update();

    // 3) analogOut CV's : 10bit to 12bit resolution!
    // TODO quantize or not, always crop!!! ?????
    analogOutBankCVs.set(0, analogInBankCVs.get(0)*4);
    analogOutBankCVs.set(1, analogInBankCVs.get(1)*4);
    analogOutBankCVs.update();

    // 4) gate out
    uint8_t dividedGateCounter = m_GateCounter / state.gateDivider;
    digitalWrite(gateOutPin, 1-(dividedGateCounter & 1));
    
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

  Serial.println("Step8x2 v0.1");

  app.Begin();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  app.Update();

  app.PrintDebug(1000);

  delay(1);//??needed??
}