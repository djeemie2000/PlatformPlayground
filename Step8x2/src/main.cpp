#include <Arduino.h>

struct DevBoard
{
  static const int clockInPin = 2;
  static const int resetInPin = 3;

  static const int gateDividerInPin = A2;
  static const int stepDividerInPin = A3;

  static const int outPinABC = 4;
  static const int outPinGate = A0;
  
  uint16_t m_Counter;
  uint8_t m_ClockHistory;
  uint8_t m_ResetHistory;

  uint16_t m_DebugCounter;
  uint32_t m_DebugMillis;

  DevBoard() : m_Counter(0)
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
    pinMode(outPinABC, OUTPUT);    
    pinMode(outPinABC+1, OUTPUT);    
    pinMode(outPinABC+2, OUTPUT);    
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

    uint16_t gateDivide = analogRead(gateDividerInPin);
    gateDivide = (gateDivide * 6) >> 10;//[0,5]
    uint16_t abcDivide = analogRead(stepDividerInPin);
    abcDivide = (abcDivide * 6) >> 10;//[0,5]

    uint16_t dividedGateCounter = m_Counter >> gateDivide;//TODO divide
    digitalWrite(outPinGate, 1-(dividedGateCounter & 1));

    uint16_t dividedABCCounter = m_Counter >> abcDivide;//TODO divide
    dividedABCCounter = dividedABCCounter >> 1;
    digitalWrite(outPinABC, dividedABCCounter & 1);
    dividedABCCounter = dividedABCCounter >> 1;
    digitalWrite(outPinABC+1, dividedABCCounter & 1);
    dividedABCCounter = dividedABCCounter >> 1;
    digitalWrite(outPinABC+2, dividedABCCounter & 1);
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