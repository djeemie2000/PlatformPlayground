#include <Arduino.h>
#include "MPR121TouchPad.h"
#include "TTP8229TouchPad.h"
#include "TouchStateOut.h"
#include "TouchInState.h"
#include "MonoHighestNotePrio.h"
#include "ChordMem.h"
#include "Arp.h"


//MPR121TouchPad g_TouchPad;
TTP8229TouchPad g_TouchPad;
TouchInState g_TouchInState1;
TouchInState g_TouchInState2;
ChordMem g_ChordMem1;
ChordMem g_ChordMem2;
MonoHighestNotePrio g_Mono1;
MonoHighestNotePrio g_Mono2;
Arp g_Arp1;
Arp g_Arp2;
TouchStateOut g_TouchStateOut1;
TouchStateOut g_TouchStateOut2;
const int gateInPin1 = A0;
const int modeInPin1 = A1;
const int gateInPin2 = A2;
const int modeInPin2 = A3;
const int debugOutPin = 2;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 Serial.println("Key8 v0.4...");

  // SPI uses pins 13=LED and 12 
  // so no led -> pinMode(LED_BUILTIN, OUTPUT);
  pinMode(gateInPin1, INPUT_PULLUP);
  pinMode(modeInPin1, INPUT_PULLUP);
  pinMode(gateInPin2, INPUT_PULLUP);
  pinMode(modeInPin2, INPUT_PULLUP);

  // debug pin 
  //  low => print debug out
  //  high (default) => no debug print
  pinMode(debugOutPin, INPUT_PULLUP);

  //g_TouchPad.Begin(PIND2);//IRQ pin D2
  g_TouchPad.Begin();
  
  g_TouchStateOut1.Begin(7, 8, 9, 10);
  g_TouchStateOut2.Begin(3, 4, 5, 6);
}

void blinkLed(int period)
{
  //digitalWrite(LED_BUILTIN, 1);
  Serial.println("Led ON");
  delay(period);
  //digitalWrite(LED_BUILTIN, 0);
  Serial.println("Led OFF");
  delay(period);
}

void testTouchPad(MPR121TouchPad& touchPad)
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

template<class TP>
void updateTouchInState(const TP& touchPad, TouchInState& touchState, const int* offsets)
{
  //TODO gate in
  touchState.m_Num = 0;
  for(int idx = 0; idx<TouchInState::Size; ++idx)
  {
    touchState.m_State[idx] = touchPad.Get(offsets[idx]);
    if(touchState.m_State[idx])
    {
      ++touchState.m_Num;
    }
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
  Serial.print("TouchIn : ");
  Serial.print(touchState.m_Num);
  Serial.print(" pressed ");
  for(int idx = 0; idx<TouchInState::Size; ++idx)
  {
    if(touchState.m_State[idx])
    {
      Serial.print(idx);
    }
  }
  Serial.println();
}

// void updateSequenceChordMemoryMode(const TouchOutState& prevState, const MPR121TouchPad& touchPad, Sequence& sequence)
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

   // touchpad left vs right
   g_TouchInState1.m_Gate = debugCounter<250;// 0<digitalRead(gateInPin1);// debugCounter<250;
   const int offsets1[] = {0,1,4,5,8,9,12,13};
   updateTouchInState(g_TouchPad, g_TouchInState1,offsets1);
   g_TouchInState2.m_Gate = debugCounter<250;// 0<digitalRead(gateInPin1);// debugCounter<250;
   const int offsets2[] = {2,3,6,7,10,11,14,15};
   updateTouchInState(g_TouchPad, g_TouchInState2, offsets2);

   g_ChordMem1.update(g_TouchInState1);
   g_ChordMem2.update(g_TouchInState2);
   g_Arp1.update(g_ChordMem1.m_State);
   g_Arp2.update(g_ChordMem2.m_State);
   g_Mono1.update(g_TouchInState1);
   g_Mono2.update(g_TouchInState2);
   
   if(digitalRead(modeInPin1))
   {
     g_TouchStateOut1.Write(g_Mono1.m_OutState);
   }
   else
   {
     g_TouchStateOut1.Write(g_Arp1.m_OutState);     
   }
   
   if(digitalRead(modeInPin2))
   {
     g_TouchStateOut2.Write(g_Mono2.m_OutState);
   }
   else
   {
     g_TouchStateOut2.Write(g_Arp2.m_OutState);     
   }
   
   ++debugCounter;
   if(500<debugCounter && !digitalRead(debugOutPin))
   {
     // print debug only ~ input low (pullup)
     debugTouchInState(g_TouchInState1);
     debugTouchInState(g_ChordMem1.m_State);
     debugTouchInState(g_TouchInState2);
     debugTouchOutState(g_Mono1.m_OutState);
     debugTouchOutState(g_Arp1.m_OutState);
     debugCounter = 0;
   }

   delay(1);// TODO what delay is needed??
  }
}
