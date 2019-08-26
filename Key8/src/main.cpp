#include <Arduino.h>
#include "CapacitiveTouchPad.h"
#include "Sequence.h"
#include "Stepper.h"

CapacitiveTouchPad g_TouchPad;

struct TouchState
{
  int numPressed;
  int selectedPad;

  TouchState() 
  : numPressed(0)
  , selectedPad(0)
  {}
};

TouchState g_TouchState;

class TouchStateOut
{
public:
  TouchStateOut() {}//undefined pins!

  void Begin(int gatePin, int bit0Pin, int bit1Pin, int bit2Pin)
  {
    m_GateOutPin = gatePin;
    m_Bit0Pin = bit0Pin;
    m_Bit1Pin = bit1Pin;
    m_Bit2Pin = bit2Pin;

    pinMode(m_GateOutPin, OUTPUT);
    pinMode(m_Bit0Pin, OUTPUT);
    pinMode(m_Bit1Pin, OUTPUT);
    pinMode(m_Bit2Pin, OUTPUT);
  }

  void Write(const TouchState& touchState)
  {
    digitalWrite(m_GateOutPin, 0<touchState.numPressed?1:0);
    digitalWrite(m_Bit0Pin, (touchState.selectedPad&1)?1:0);
    digitalWrite(m_Bit1Pin, (touchState.selectedPad&2)?1:0);
    digitalWrite(m_Bit2Pin, (touchState.selectedPad&4)?1:0);
  }

  private:
    int m_GateOutPin;
    int m_Bit0Pin;
    int m_Bit1Pin;
    int m_Bit2Pin;
};

TouchStateOut g_TouchStateOut;

Sequence g_Sequence;
Stepper g_Stepper;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
 Serial.println("Key8 v0.2...");

  pinMode(LED_BUILTIN, OUTPUT);

  g_TouchPad.Begin(PIND2);//IRQ pin D2
  g_TouchStateOut.Begin(11, 8, 9, 10);
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

void updateTouchStateMonoMode(const CapacitiveTouchPad& touchPad, TouchState& touchState)
{
  // call read elsewhere!  touchPad.Read();
  touchState.numPressed = 0;//reset num pressed count
  // hold selected pad when none are pushed
  // highest pad priority => count up and set selected
  for(int pad = 0; pad<8; ++pad)//limit to pad [0-7]
  {
    if(touchPad.IsPushed(pad))
    {
      ++touchState.numPressed;
      touchState.selectedPad = pad;
    }
  }
}

void debugTouchState(const TouchState& touchState)
{
  Serial.print(touchState.numPressed);
  Serial.print(" pressed, current=");
  Serial.println(touchState.selectedPad);
}

void updateSequenceChordMemoryMode(const TouchState& prevState, const CapacitiveTouchPad& touchPad, Sequence& sequence)
{
  // check if first clicked => sequence.Clear();
  bool clearUponClicked = (0==prevState.numPressed);

  for(int pad = 0; pad<8; ++pad)
  {
    if(touchPad.IsClicked(pad))
    {
      if(clearUponClicked)
      {
        sequence.Clear();
        clearUponClicked = false;
      }
      const int gate = 1;//no skips in this mode
      sequence.Append(pad, gate);
    }
  }
}

void debugSequence(const Sequence& sequence)
{
  Serial.print("Seq l=");
  Serial.println(sequence.Length());
}

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

   updateSequenceChordMemoryMode(g_TouchState, g_TouchPad, g_Sequence);

   updateTouchStateMonoMode(g_TouchPad, g_TouchState);
   g_TouchStateOut.Write(g_TouchState);

   ++debugCounter;
   if(500<debugCounter)
   {
     debugTouchState(g_TouchState);
     debugSequence(g_Sequence);
     debugCounter = 0;
   }
   delay(1);// TODO what delay is needed??
  }
}
