#include <Arduino.h>
#include <SPI.h>

#include "analoginbank.h"
#include "shiftoutbank.h"

// 2nd ABC out + 2nd clock divider pot
//TODO seq length pot x2
//TODO cvclock on 2nd gate ~pot or ~seq 2???
//TODO chaining of 8 + 8 = 16?

struct DevBoard
{
  static const int clockInPin = 2;
  static const int resetInPin = 3;

  static const int gateDividerInPin = A2;
  static const int stepDividerInPin1 = A3;
  static const int stepDividerInPin2 = A4;
  static const int stepLengthInPin1 = A5;
  static const int stepLengthInPin2 = A6;
  
  static const int outPinGate = 7;
  
  uint16_t m_Counter;
  uint8_t m_ClockHistory;
  uint8_t m_ResetHistory;

  uint16_t m_DebugCounter;
  uint32_t m_DebugMillis;

  AnalogInBank analogInBank;
  ShiftOutBank shiftOutBank;

  DevBoard() 
  : m_Counter(0)
  , analogInBank(A2, A3, A4, A5, A6)
  , shiftOutBank(10)
  {}

  void Begin()
  {
    m_DebugCounter = 0;

    m_Counter= 0;
    m_ClockHistory = 0;
    m_ResetHistory = 0;

    pinMode(clockInPin, INPUT_PULLUP);
    pinMode(resetInPin, INPUT_PULLUP);

    pinMode(outPinGate, OUTPUT);    

    analogInBank.begin();
    shiftOutBank.begin();
  }

  void Update()
  {
    ++m_DebugCounter;

    m_ClockHistory = (m_ClockHistory<<1) | digitalRead(clockInPin);
    m_ResetHistory = (m_ResetHistory<<1) | digitalRead(resetInPin);
    
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

    analogInBank.updateOne();

    const uint16_t numDividers = 10; 
    uint16_t gateDivide = analogInBank.get(0);// analogRead(gateDividerInPin);
    gateDivide = (gateDivide * numDividers) >> 10;
    uint16_t abcDivide1 = analogInBank.get(1);// analogRead(stepDividerInPin1);
    abcDivide1 = (abcDivide1 * numDividers) >> 10;
    uint16_t abcDivide2 = analogInBank.get(2);//analogRead(stepDividerInPin2);
    abcDivide2 = (abcDivide2 * numDividers) >> 10;
    uint16_t stepLength1 = analogInBank.get(3);//analogRead(stepLengthInPin1);
    stepLength1 = 1 + (stepLength1>>7);//[1,8]
    uint16_t stepLength2 = analogInBank.get(4);//analogRead(stepLengthInPin2);
    stepLength2 = 1 + (stepLength2>>7);//[1,8]

//    uint16_t dividers[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
//    uint16_t dividers[] = {1, 2, 2, 4, 4, 8, 8, 16, 16, 32};
    uint16_t dividers[] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24};

    {
        uint16_t dividedGateCounter = m_Counter / dividers[gateDivide];
        digitalWrite(outPinGate, 1-(dividedGateCounter & 1));
    }
    {
        uint16_t dividedABCCounter = m_Counter / dividers[abcDivide1];
        dividedABCCounter = dividedABCCounter >> 1;
        dividedABCCounter= dividedABCCounter % stepLength1;

        shiftOutBank.set(0, dividedABCCounter & 1);
        dividedABCCounter = dividedABCCounter >> 1;
        shiftOutBank.set(1, dividedABCCounter & 1);
        dividedABCCounter = dividedABCCounter >> 1;
        shiftOutBank.set(2, dividedABCCounter & 1);
    }
    {
        uint16_t dividedABCCounter2 = m_Counter / dividers[abcDivide2];
        dividedABCCounter2 = dividedABCCounter2 >> 1;
        dividedABCCounter2 = dividedABCCounter2 % stepLength2;

        shiftOutBank.set(4, dividedABCCounter2 & 1);
        dividedABCCounter2 = dividedABCCounter2 >> 1;
        shiftOutBank.set(5, dividedABCCounter2 & 1);
        dividedABCCounter2 = dividedABCCounter2 >> 1;
        shiftOutBank.set(6, dividedABCCounter2 & 1);
    }
    shiftOutBank.update();
  }

  void PrintDebug(uint16_t period)
  {
    if(m_DebugCounter >= period)
    {
      uint32_t newMillis = millis();

      Serial.print(m_Counter, BIN);
      Serial.print(" ");
      Serial.print(m_ClockHistory, BIN);
      Serial.print(" ");
      Serial.print(m_DebugCounter);
      Serial.print(" ");
      Serial.println(newMillis - m_DebugMillis);
      
      m_DebugCounter = 0;
      m_DebugMillis = newMillis;
    }

  }
};

DevBoard devBoard;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Step8x2 v0.1");

  devBoard.Begin();
}

void loop() 
{
  // put your main code here, to run repeatedly:

  //read clock in
  // if rising => counter +=2
  // counter lowest bit ~clock
  // gate out ~ lowest bit ???
  //  TODO shift counter ~AIn
  // ABC outputs ~counter bits
  //  TODO shift counter ~AIn

  devBoard.Update();

  devBoard.PrintDebug(1000);

  delay(1);//??needed??

}