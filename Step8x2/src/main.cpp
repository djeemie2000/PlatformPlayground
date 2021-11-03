#include <Arduino.h>
#include <SPI.h>

#include "multiplexeranaloginbank.h"
#include "digitaloutbank.h"
//#include "CVClockState.h"
#include "gatecounter.h"
#include "stepcounter.h"


// struct Step8x2State
// {
//   static const int NumSteps = 8;
  
//   uint16_t stepA;
//   uint16_t stepB;

//   uint16_t gateDivider;
//   uint16_t gateDuration;
  
//   uint16_t stepDividerA;
//   uint16_t stepLengthA;
//   int enableA;
  
//   uint16_t stepDividerB;
//   uint16_t stepLengthB;
//   int enableB;
  
//   bool chaining;

//   Step8x2State()
//    : stepA(0)
//    , stepB(0)
//    , gateDivider(1)
//    , gateDuration(256)
//    , stepDividerA(1)
//    , stepLengthA(NumSteps)
//    , enableA(1)
//    , stepDividerB(1)
//    , stepLengthB(NumSteps)
//    , enableB(1)
//    , chaining(false)
//   {
    
//   }

//   void AdvanceA()
//   {
//     ++stepA;
//     if(stepLengthA<=stepA)
//     {
//       stepA = 0;
//     }
//   }

//   void AdvanceB()
//   {
//     ++stepB;
//     if(stepLengthB<=stepB)
//     {
//       stepB = 0;
//     }

//   }
// };

// void PrintState(Step8x2State& state)
// {
//   Serial.print("A / ");
//   Serial.print(state.stepDividerA);
//   Serial.print(" L ");
//   Serial.print(state.stepLengthA);
//   Serial.print(" S ");
//   Serial.print(state.stepA);
//   Serial.print(" E ");
//   Serial.println(state.enableA);

//   Serial.print("B / ");
//   Serial.print(state.stepDividerB);
//   Serial.print(" L ");
//   Serial.print(state.stepLengthB);
//   Serial.print(" S ");
//   Serial.print(state.stepB);
//   Serial.print(" E ");
//   Serial.println(state.enableB);

//   Serial.print("G / ");
//   Serial.println(state.gateDivider);

//   Serial.print("A->B : ");
//   Serial.println(state.chaining);
// }


struct Step8x2App
{
  static const int clockInPin = 2;
  static const int resetInPin = 3;

  // 4 gate out pins 4,5,6,7  
  // analog in bank for controls -> read 1 at a time A3 .. A7

  uint8_t m_Cntr;
  uint8_t m_ClockHistory;
  uint8_t m_ResetHistory;

  uint16_t m_DebugCounter;
  uint32_t m_DebugMillis;

  static const int NumGates = 4;
  static const int NumCVs = 6;
  GateCounter m_GateCounter[NumGates];
  StepCounter m_StepCounter[NumCVs];

  //DigitalOutBank digitalOutSelect; // ABC CV row 1, ABC CV row 2, A/B output 1, A/B output 2 
  DigitalOutBank digitalOutGate; 
  MultiplexerAnaloginIn analogInBankControls;


  Step8x2App() 
  : m_Cntr(0)
  , m_GateCounter()
  , digitalOutGate()
  , analogInBankControls()
  {}

  void Begin()
  {
    m_Cntr = 0;
    m_DebugCounter = 0;
    m_DebugMillis = 0;

    m_ClockHistory = 0;
    m_ResetHistory = 0;

    pinMode(clockInPin, INPUT_PULLUP);
    pinMode(resetInPin, INPUT_PULLUP);

    analogInBankControls.begin(A3, A4, A5, A6, A7);
    digitalOutGate.begin(4,5,6,7);
  }

  uint16_t ControlToDivider(int cv)
  {
    static const uint16_t numDividers = 10; 
//    uint16_t dividers[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
//    uint16_t dividers[] = {1, 2, 2, 4, 4, 8, 8, 16, 16, 32};
    const uint16_t dividers[] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24};

    uint16_t gateDividerIdx = cv;
    gateDividerIdx = (gateDividerIdx * numDividers) >> 10;
    return dividers[gateDividerIdx];
  }

  uint16_t ControlToLength(int cv)
  {
    uint16_t length = cv;
    length = 1 + (length>>7);//[1,8]
    return length;
  }

  void Update()
  {
    ++m_DebugCounter;

    m_ClockHistory = (m_ClockHistory<<1) | digitalRead(clockInPin);
    m_ResetHistory = (m_ResetHistory<<1) | digitalRead(resetInPin);
    
    bool tick = false;
    bool tickGate = false;
    bool reset = false;
    if((m_ResetHistory & 0x03) == 1)
    {
      //reset rising
      reset  = true;
    }
    else if((m_ClockHistory & 0x03) == 0x01)
    {
      //rising
      if(m_Cntr & 0x01)
      {
        ++m_Cntr;
        tickGate = true;
      }
      tick = true;
    }
    else if((m_ClockHistory & 0x03) == 0x02)
    {
      //falling
      if(!(m_Cntr & 0x01))
      {
        ++m_Cntr;
        tickGate = true;
      }
    }

    analogInBankControls.updateOne();

    m_StepCounter[0].SetDivider(ControlToDivider(analogInBankControls.Get(0, 0)));
    m_StepCounter[0].SetLength(ControlToLength(analogInBankControls.Get(0, 1)));
    m_StepCounter[1].SetDivider(ControlToDivider(analogInBankControls.Get(0, 2)));
    m_StepCounter[1].SetLength(ControlToLength(analogInBankControls.Get(0, 3)));
    m_StepCounter[2].SetDivider(ControlToDivider(analogInBankControls.Get(0, 4)));
    m_StepCounter[2].SetLength(ControlToLength(analogInBankControls.Get(0, 5)));
    m_GateCounter[0].SetDivider(ControlToDivider(analogInBankControls.Get(0, 6)));
    m_GateCounter[1].SetDivider(ControlToDivider(analogInBankControls.Get(0, 7)));

    m_StepCounter[3].SetDivider(ControlToDivider(analogInBankControls.Get(1, 0)));
    m_StepCounter[3].SetLength(ControlToLength(analogInBankControls.Get(1, 1)));
    m_StepCounter[4].SetDivider(ControlToDivider(analogInBankControls.Get(1, 2)));
    m_StepCounter[4].SetLength(ControlToLength(analogInBankControls.Get(1, 1)));
    m_StepCounter[5].SetDivider(ControlToDivider(analogInBankControls.Get(1, 4)));
    m_StepCounter[5].SetLength(ControlToLength(analogInBankControls.Get(1, 5)));
    m_GateCounter[2].SetDivider(ControlToDivider(analogInBankControls.Get(1, 6)));
    m_GateCounter[3].SetDivider(ControlToDivider(analogInBankControls.Get(1, 7)));    

    if(reset)
    {
        m_GateCounter[0].Reset();
        m_GateCounter[1].Reset();
        m_GateCounter[2].Reset();
        m_GateCounter[3].Reset();

        m_StepCounter[0].Reset();
        m_StepCounter[1].Reset();
        m_StepCounter[2].Reset();
        m_StepCounter[3].Reset();
        m_StepCounter[4].Reset();
        m_StepCounter[5].Reset();
    }

    if(tickGate)
    {
      m_GateCounter[0].Tick();
      m_GateCounter[1].Tick();
      m_GateCounter[2].Tick();
      m_GateCounter[3].Tick();
    }

    if(tick)
    {
      m_StepCounter[0].Tick();
      m_StepCounter[1].Tick();
      m_StepCounter[2].Tick();
      m_StepCounter[3].Tick();
      m_StepCounter[4].Tick();
      m_StepCounter[5].Tick();
    }

    // set gate outs + update
    digitalOutGate.set(0, m_GateCounter[0].Gate());
    digitalOutGate.set(1, m_GateCounter[1].Gate());
    digitalOutGate.set(2, m_GateCounter[2].Gate());
    digitalOutGate.set(3, m_GateCounter[3].Gate());

    digitalOutGate.update(0);
    // state.stepLengthA = analogInBankControls.get(3);
    // state.stepLengthA = 1 + (state.stepLengthA>>7);//[1,8]
      
    // state.stepLengthB = analogInBankControls.get(4);
    // state.stepLengthB = 1 + (state.stepLengthB>>7);//[1,8]

    // if pot at max => chaining
    // state.chaining = (1016<analogInBankControls.get(3));

    // 1) shift out select ABC
    // 4) Gate out
    // if(tick)
    // {
    //   bool advanceA = (m_StepCounterA % state.stepDividerA) == 0;
    //   bool endOfSequenceA = advanceA && (state.stepA +1 >= state.stepLengthA);

    //   bool advanceB = (m_StepCounterB % state.stepDividerB) == 0;
    //   bool endOfSequenceB = advanceB && (state.stepB +1 >= state.stepLengthB);
           
    //   if(state.chaining)
    //   {
    //     if(state.enableA && advanceA)
    //     {
    //         if(endOfSequenceA)
    //         {
    //             state.enableA = 0;
    //             state.enableB = 1;
    //         }
    //     }
    //     else if(state.enableB && advanceB)
    //     {
    //         if(endOfSequenceB)
    //         {
    //           state.enableA = 1;
    //           state.enableB = 0;
    //         }
    //     }
    //   }
    //   else
    //   {
    //     //not chaining
    //     if(state.enableA && advanceA)
    //     {
    //         if(endOfSequenceB)
    //         {
    //             state.enableA = 1;
    //             state.enableB = 1;
    //         }
    //     }
    //     if(state.enableB && advanceB)
    //     {
    //         if(endOfSequenceB)
    //         {
    //           state.enableA = 1;
    //           state.enableB = 1;
    //         }
    //     }
    //   }

    //   if(state.enableA && advanceA)
    //   {
    //     state.AdvanceA();//state.stepA = stepA;

    //     uint16_t stepA = state.stepA;
    //     digitalOutSelect.set(0, stepA & 1);
    //     stepA = stepA >> 1;
    //     digitalOutSelect.set(1, stepA & 1);
    //     stepA = stepA >> 1;
    //     digitalOutSelect.set(2, stepA & 1);
    //   }

    //   if(state.enableB && advanceB)
    //   {
    //     state.AdvanceB();//state.stepB = stepB;

    //     uint16_t stepB = state.stepB;
    //     digitalOutSelect.set(3, stepB & 1);
    //     stepB = stepB >> 1;
    //     digitalOutSelect.set(4, stepB & 1);
    //     stepB = stepB >> 1;
    //     digitalOutSelect.set(5, stepB & 1);
    //   }    
      

      
    // }

    // 0 <-> A, 1 <-> B
    // int selectAB1 = state.enableA ? 0 : 1; 
    // int selectAB2 = state.enableB ? 1 : 0; 
    // digitalOutSelect.set(6, selectAB1);
    // digitalOutSelect.set(7, selectAB2); 
     

    //digitalOutSelect.update(0);

  }

  void PrintDebug(uint16_t period)
  {
    if(m_DebugCounter >= period)
    {
      uint32_t newMillis = millis();

      Serial.print(m_DebugCounter);
      Serial.print(" ");
      Serial.println(newMillis - m_DebugMillis);

      Serial.println("Controls:");
      printAnalogInBank(analogInBankControls);
      
      //PrintState
      Serial.println("Gate:");
      m_GateCounter[0].PrintState();
      m_GateCounter[1].PrintState();
      m_GateCounter[2].PrintState();
      m_GateCounter[3].PrintState();

      Serial.println("Step:");
      m_StepCounter[0].PrintState();
      m_StepCounter[1].PrintState();
      m_StepCounter[2].PrintState();
      m_StepCounter[3].PrintState();
      m_StepCounter[4].PrintState();
      m_StepCounter[5].PrintState();

      Serial.println();

      m_DebugCounter = 0;
      m_DebugMillis = newMillis;
    }

  }
};

Step8x2App app;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Step8x2 v0.5");

  app.Begin();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  app.Update();

  app.PrintDebug(1000);

  delay(1);//??needed??
}