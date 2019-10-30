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
TouchInState g_TouchInState;
TouchInState g_TouchInState2;
ChordMem g_ChordMem;
MonoHighestNotePrio g_Mono;
Arp g_Arp;
TouchStateOut g_TouchStateOut;
TouchStateOut g_TouchStateOutArp;
const int gateInPin = 3;


void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 Serial.println("Key8 v0.4...");

  // SPI uses pins 13=LED and 12 
  //  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(gateInPin, INPUT_PULLUP);

  //g_TouchPad.Begin(PIND2);//IRQ pin D2
  g_TouchPad.Begin();
  
  g_TouchStateOut.Begin(11, 8, 9, 10);
  g_TouchStateOutArp.Begin(7, 4, 5, 6);
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
void updateTouchInState(const TP& touchPad, TouchInState& touchState, int offset = 0)
{
  //TODO gate in
  touchState.m_Num = 0;
  for(int idx = 0; idx<TouchInState::Size; ++idx)
  {
    touchState.m_State[idx] = touchPad.Get(idx+offset);
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

//   while(true)
//   {
//     //FindI2CDevices();
//     uint16_t keys = SPI.transfer16(0);
//     uint16_t invKeys = ~keys;
//     Serial.println(keys, BIN);
//     Serial.println(invKeys, BIN);

//     blinkLed(300);
// //    blinkLed(300);
//   }

  int debugCounter = 0;
  while(true)
  {
   g_TouchPad.Read();
//   testTouchPad(g_TouchPad);

   g_TouchInState.m_Gate = debugCounter<250;// 0<digitalRead(gateInPin);// debugCounter<250;
   updateTouchInState(g_TouchPad, g_TouchInState);
   g_TouchInState2.m_Gate = debugCounter<250;// 0<digitalRead(gateInPin);// debugCounter<250;
   updateTouchInState(g_TouchPad, g_TouchInState2, 8);

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
     debugTouchInState(g_TouchInState2);
     debugTouchOutState(g_Mono.m_OutState);
     debugTouchOutState(g_Arp.m_OutState);
     //debugSequence(g_Sequence);
     debugCounter = 0;
   }
   delay(1);// TODO what delay is needed??
  }
}
