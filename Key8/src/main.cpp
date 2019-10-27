#include <Arduino.h>
#include "CapacitiveTouchPad.h"
#include "Sequence.h"
#include "Stepper.h"
#include "TouchStateOut.h"
#include "TouchInState.h"

CapacitiveTouchPad g_TouchPad;
TouchInState g_TouchInState;
TouchInState g_TouchInStateChordMem;
TouchOutState g_TouchOutState;
TouchOutState g_TouchOutStateArp;
TouchStateOut g_TouchStateOut;
TouchStateOut g_TouchStateOutArp;

Sequence g_Sequence;
Stepper g_Stepper;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 Serial.println("Key8 v0.2...");

  pinMode(LED_BUILTIN, OUTPUT);

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

void updateTouchOutStateMono(const TouchInState& inState, TouchOutState& outState)
{
  outState.numPressed = 0;//reset num pressed count
  // hold selected pad when none are pushed
  // highest pad priority => count up and set selected
  for(int pad = 0; pad<TouchInState::Size; ++pad)
  {
    if(inState.m_State[pad])
    {
      ++outState.numPressed;
      outState.selectedPad = pad;
    }
  }
}

void updateTouchInStateChordMem(const TouchInState& inState, TouchInState& outState)
{
  int prevNumPressed = 0;//TODO member?
  for(int pad = 0; pad<TouchInState::Size; ++pad)
  {
    if(inState.m_State[pad])
    {
      ++prevNumPressed;
    }
  }

  for(int pad = 0; pad<TouchInState::Size; ++pad)
  {
    // hold any previously pressed until 'reset' by pressing a first key again
    if(inState.m_State[pad])
    {
      outState.m_State[pad] = true;
    }
    else if(0==prevNumPressed)
    {
      outState.m_State[pad] = false;
    }
  }
}

void advanceTouchOutStateArp(const TouchInState& inState, TouchOutState& outState)
{
  // advance outState to next selected 
  int selected = outState.selectedPad;
  for(int idx = 1; idx<TouchInState::Size; ++idx)
  {
    ++selected;
    if(TouchInState::Size<=selected)
    {
      selected = 0;
    }
    if(inState.m_State[selected])
    {
      outState.selectedPad = selected;
      break;
    }
  }
}

// void updateTouchStateMonoMode(const CapacitiveTouchPad& touchPad, TouchOutState& touchState)
// {
//   // call read elsewhere!  touchPad.Read();
//   touchState.numPressed = 0;//reset num pressed count
//   // hold selected pad when none are pushed
//   // highest pad priority => count up and set selected
//   for(int pad = 0; pad<8; ++pad)//limit to pad [0-7]
//   {
//     if(touchPad.IsPushed(pad))
//     {
//       ++touchState.numPressed;
//       touchState.selectedPad = pad;
//     }
//   }
// }

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

   //updateSequenceChordMemoryMode(g_TouchOutState, g_TouchPad, g_Sequence);

   updateTouchInState(g_TouchPad, g_TouchInState);
   updateTouchInStateChordMem(g_TouchInState, g_TouchInStateChordMem);

  //TODO gate in
  if(debugCounter==200)
  {
    advanceTouchOutStateArp(g_TouchInStateChordMem, g_TouchOutStateArp);
  }
   updateTouchOutStateMono(g_TouchInState, g_TouchOutState);
   
   g_TouchStateOut.Write(g_TouchOutState);
   g_TouchStateOutArp.Write(g_TouchOutStateArp);

   ++debugCounter;
   if(500<debugCounter)
   {
     debugTouchInState(g_TouchInState);
     debugTouchInState(g_TouchInStateChordMem);
     debugTouchOutState(g_TouchOutState);
     debugTouchOutState(g_TouchOutStateArp);
     //debugSequence(g_Sequence);
     debugCounter = 0;
   }
   delay(1);// TODO what delay is needed??
  }
}
