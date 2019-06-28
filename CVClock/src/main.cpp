#include "CVClock.h"
#include <Arduino.h>

CVClock g_Clock;
CVClock g_Clock2;
CVClock g_Clock3;
CVClock g_Clock4;
CVClock g_Clock5;

int g_DebugCntr;
uint32_t g_DebugMillis;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("CVClock 0.3 start...");

  g_Clock.Begin(11, 2, A4);
  g_Clock2.Begin(10, 3, A3);
  g_Clock3.Begin(9, 4, A2);
  g_Clock4.Begin(8, 5, A1);
  g_Clock5.Begin(7, 6, A0);
  pinMode(12, INPUT_PULLUP);

  g_DebugCntr = 0;
  g_DebugMillis = 0;
}

void loop()
{
  // put your main code here, to run repeatedly:
  // TODO alternate analog reads?
  g_Clock.ReadDuration();
  g_Clock.Tick();
  g_Clock2.ReadDuration();
  g_Clock2.Tick();
  g_Clock3.ReadDuration();
  g_Clock3.Tick();
  g_Clock4.ReadDuration();
  g_Clock4.Tick();
  g_Clock5.ReadDuration();
  g_Clock5.Tick();
  //  delay(1);//not needed

  //  only debug if debug input is low (pullup digital in)
  if (!digitalRead(12))
  {
    ++g_DebugCntr;
    if (1000 < g_DebugCntr)
    {
      // TODO stopwatch class
      Serial.println();
      Serial.println("-");
      uint32_t time = millis();
      uint32_t elapsed = time - g_DebugMillis;
      g_Clock.debugOut(elapsed);
      g_Clock2.debugOut(elapsed);
      g_Clock3.debugOut(elapsed);
      g_Clock4.debugOut(elapsed);
      g_Clock5.debugOut(elapsed);
      g_DebugMillis = time;
      g_DebugCntr = 0;
    }
  }
}
