#include <Arduino.h>
#include "CapacitiveTouchPad.h"
#include "Sequence.h"
#include "Stepper.h"
#include "TouchStateOut.h"
#include "TouchInState.h"

struct ChordMem
{
  TouchInState m_State;
  int m_NumPressed;

  ChordMem();
  void update(const TouchInState& inState);
};

ChordMem::ChordMem() : m_State(), m_NumPressed(0) 
{
}

void ChordMem::update(const TouchInState& inState)
{
  int prevNumPressed = m_NumPressed;
  int m_NumPressed = 0;
  for(int pad = 0; pad<TouchInState::Size; ++pad)
  {
    if(inState.m_State[pad])
    {
      ++m_NumPressed;
    }
  }
  bool resetHold = 0==prevNumPressed && 0<m_NumPressed;

  for(int pad = 0; pad<TouchInState::Size; ++pad)
  {
    // hold any previously pressed until 'reset' by pressing a first key again
    if(inState.m_State[pad])
    {
      m_State.m_State[pad] = true;
    }
    else if(resetHold)
    {
      m_State.m_State[pad] = false;
    }
  }

  m_State.m_Gate = inState.m_Gate;
}

struct Arp
{
  TouchOutState m_OutState;
  bool m_Gate;
  
  Arp();
  void update(const TouchInState& inState);
};

Arp::Arp() : m_OutState(), m_Gate(false)
{}

void Arp::update(const TouchInState& inState)
{
  bool advance = inState.m_Gate && !m_Gate;
  m_OutState.numPressed  = inState.m_Gate?1:0;
  m_Gate = inState.m_Gate;

  if(advance)
  {
    // advance outState to next selected 
    int selected = m_OutState.selectedPad;
    for(int idx = 1; idx<TouchInState::Size; ++idx)
    {
      ++selected;
      if(TouchInState::Size<=selected)
      {
        selected = 0;
      }
      if(inState.m_State[selected])
      {
        m_OutState.selectedPad = selected;
        break;
      }
    }
  }
}

struct MonoHighestPriority
{
  TouchOutState m_OutState;

  MonoHighestPriority();
  void update(const TouchInState& inState);
};

MonoHighestPriority::MonoHighestPriority() : m_OutState()
{}

void MonoHighestPriority::update(const TouchInState& inState)
{
  m_OutState.numPressed = 0;//reset num pressed count
  // hold selected pad when none are pushed
  // highest pad priority => count up and set selected
  for(int pad = 0; pad<TouchInState::Size; ++pad)
  {
    if(inState.m_State[pad])
    {
      ++m_OutState.numPressed;
      m_OutState.selectedPad = pad;
    }
  }
}


CapacitiveTouchPad g_TouchPad;
TouchInState g_TouchInState;
ChordMem g_ChordMem;
MonoHighestPriority g_Mono;
Arp g_Arp;
TouchStateOut g_TouchStateOut;
TouchStateOut g_TouchStateOutArp;
const int gateInPin = 3;


void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 Serial.println("Key8 v0.2...");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(gateInPin, INPUT_PULLUP);

  g_TouchPad.Begin(PIND2);//IRQ pin D2
  g_TouchStateOut.Begin(11, 8, 9, 10);
  g_TouchStateOutArp.Begin(7, 4, 5, 6);
}

void blinkLed(int period)
{
  digitalWrite(LED_BUILTIN, 1);
  Serial.println("Led ON");
  delay(period);
  digitalWrite(LED_BUILTIN, 0);
  Serial.println("Led OFF");
  delay(period);
}

void testTouchPad(CapacitiveTouchPad& touchPad)
{
  for(int pad = 0; pad<touchPad.GetNumPads(); ++pad)
  {
    if(touchPad.IsClicked(pad))
    {
      Serial.print(pad);
      Serial.println(" was clicked");
    }
    else if(touchPad.IsReleased(pad))
    {
      Serial.print(pad);
      Serial.println(" was released");
    }
  }
}

void updateTouchInState(const CapacitiveTouchPad& touchPad, TouchInState& touchState)
{
  //TODO gate in
  for(int idx = 0; idx<TouchInState::Size; ++idx)
  {
    touchState.m_State[idx] = touchPad.Get(idx);
  }
}


void debugTouchOutState(const TouchOutState& touchState)
{
  Serial.print("TouchOut : ");
  Serial.print(touchState.numPressed);
  Serial.print(" pressed, current=");
  Serial.println(touchState.selectedPad);
}

void debugTouchInState(const TouchInState& touchState)
{
  Serial.print("TouchIn : pressed");
  for(int idx = 0; idx<TouchInState::Size; ++idx)
  {
    if(touchState.m_State[idx])
    {
      Serial.print(idx);
    }
  }
  Serial.println();
}

// void updateSequenceChordMemoryMode(const TouchOutState& prevState, const CapacitiveTouchPad& touchPad, Sequence& sequence)
// {
//   // check if first clicked => sequence.Clear();
//   bool clearUponClicked = (0==prevState.numPressed);

//   for(int pad = 0; pad<8; ++pad)
//   {
//     if(touchPad.IsClicked(pad))
//     {
//       if(clearUponClicked)
//       {
//         sequence.Clear();
//         clearUponClicked = false;
//       }
//       const int gate = 1;//no skips in this mode
//       sequence.Append(pad, gate);
//     }
//   }
// }

// void debugSequence(const Sequence& sequence)
// {
//   Serial.print("Seq l=");
//   Serial.println(sequence.Length());
// }

void loop() {
  // put your main code here, to run repeatedly:
  blinkLed(300);
  blinkLed(300);
  blinkLed(800);

  int debugCounter = 0;
  while(true)
  {
   g_TouchPad.Read();
//   testTouchPad(g_TouchPad);

   g_TouchInState.m_Gate = 0<digitalRead(gateInPin);// debugCounter<250;
   updateTouchInState(g_TouchPad, g_TouchInState);

   g_ChordMem.update(g_TouchInState);
   g_Arp.update(g_ChordMem.m_State);

   g_Mono.update(g_TouchInState);
   
   g_TouchStateOut.Write(g_Mono.m_OutState);
   g_TouchStateOutArp.Write(g_Arp.m_OutState);

   ++debugCounter;
   if(500<debugCounter)
   {
     //TODO print debug ~ input low (pullup)
     debugTouchInState(g_TouchInState);
     debugTouchInState(g_ChordMem.m_State);
     debugTouchOutState(g_Mono.m_OutState);
     debugTouchOutState(g_Arp.m_OutState);
     //debugSequence(g_Sequence);
     debugCounter = 0;
   }
   delay(1);// TODO what delay is needed??
  }
}
