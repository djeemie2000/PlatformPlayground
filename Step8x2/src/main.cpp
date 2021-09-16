#include <Arduino.h>
#include <SPI.h>

#include "analoginbank.h"
#include "shiftoutbank.h"
#include "analogoutbank.h"

//TODO chaining of 8 + 8 = 16?
//TODO quantize CV out
//TODO cvclock on 2nd gate ~ CV input
//    take clock divider into account!!!
//TODO button/pot controls state + step CV -> state struct
//    => possible to store/recall settings + step CV (?) 
//    => debug print state
//TODO hold input => no step advance

struct Step8x2State
{
  static const int NumSteps = 8;
  
  int stepCV1[NumSteps];
  int stepCV2[NumSteps];
  int gateDurationCV;

  int step1;
  int step2;

  uint16_t gateDivider;
  
  uint16_t stepDivider1;
  uint16_t stepLength1;
  bool quantize1;

  uint16_t stepDivider2;
  uint16_t stepLength2;
  bool quantize2;

  bool chaining;

  Step8x2State()
   : gateDurationCV(512)
   , step1(0)
   , step2(0)
   , gateDivider(1)
   , stepDivider1(1)
   , stepLength1(NumSteps)
   , quantize1(false)
   , stepDivider2(1)
   , stepLength2(NumSteps)
   , quantize2(false)
   , chaining(false)
  {
    for(int idx = 0; idx<NumSteps;++idx)
    {
      stepCV1[idx] = 0;
      stepCV2[idx] = 0;
    }
  }

};

void PrintState(Step8x2State& state)
{
  Serial.print("A / ");
  Serial.print(state.stepDivider1);
  Serial.print(" L ");
  Serial.print(state.stepLength1);
  Serial.print(" Q ");
  Serial.print(state.quantize1);
  Serial.print(" S ");
  Serial.println(state.step1);

  Serial.print("B / ");
  Serial.print(state.stepDivider2);
  Serial.print(" L ");
  Serial.print(state.stepLength2);
  Serial.print(" Q ");
  Serial.print(state.quantize2);
  Serial.print(" S ");
  Serial.println(state.step2);

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
  
  
  uint16_t m_Counter;
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
  : m_Counter(0)
  , analogInBankControls(A2, A3, A4, A5, A6)
  , analogInBankCVs(A0, A1, A7)
  , shiftOutBank(9)// analogOutBank uses CS pin 9 for first DAC!
  , analogOutBankCVs()
  , state()
  {}

  void Begin()
  {
    m_DebugCounter = 0;

    m_Counter= 0;
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
      m_Counter = 0;
    }
    else if((m_ClockHistory & 0x03) == 0x01)
    {
      //rising
      if(m_Counter & 0x01)
      {
        ++m_Counter;
      }
    }
    else if((m_ClockHistory & 0x03) == 0x02)
    {
      //falling
      if(!(m_Counter & 0x01))
      {
        ++m_Counter;
      }
    }

    analogInBankControls.updateOne();

    const uint16_t numDividers = 10; 
//    uint16_t dividers[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
//    uint16_t dividers[] = {1, 2, 2, 4, 4, 8, 8, 16, 16, 32};
    uint16_t dividers[] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24};

    state.gateDivider = analogInBankControls.get(0);
    state.gateDivider = (state.gateDivider * numDividers) >> 10;

    state.stepDivider1 = analogInBankControls.get(1);
    state.stepDivider1 = (state.stepDivider1 * numDividers) >> 10;
    
    state.stepDivider2 = analogInBankControls.get(2);
    state.stepDivider2 = (state.stepDivider2 * numDividers) >> 10;

    state.stepLength1 = analogInBankControls.get(3);
    state.stepLength1 = 1 + (state.stepLength1>>7);//[1,8]
    state.stepLength2 = analogInBankControls.get(4);
    state.stepLength2 = 1 + (state.stepLength2>>7);//[1,8]


    // 1) shift out select ABC
    // 2) read CVs
    // 3) analogOut CVs
    // 4) Gate out
    {
        uint16_t step1 = (m_Counter >> 1) / dividers[state.stepDivider1];
//        step1 = step1 >> 1;
        step1 = step1 % state.stepLength1;
        state.step1 = step1;

        shiftOutBank.set(0, step1 & 1);
        step1 = step1 >> 1;
        shiftOutBank.set(1, step1 & 1);
        step1 = step1 >> 1;
        shiftOutBank.set(2, step1 & 1);
    }
    {
        uint16_t step2 = (m_Counter >> 1) / dividers[state.stepDivider2];
//        step2 = step2 >> 1;
        step2 = step2 % state.stepLength2;
        state.step2 = step2;

        shiftOutBank.set(4, step2 & 1);
        step2 = step2 >> 1;
        shiftOutBank.set(5, step2 & 1);
        step2 = step2 >> 1;
        shiftOutBank.set(6, step2 & 1);
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
    uint16_t dividedGateCounter = m_Counter / dividers[state.gateDivider];
    digitalWrite(gateOutPin, 1-(dividedGateCounter & 1));
    
  }

  void PrintDebug(uint16_t period)
  {
    if(m_DebugCounter >= period)
    {
      uint32_t newMillis = millis();

      Serial.print(m_Counter, BIN);
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