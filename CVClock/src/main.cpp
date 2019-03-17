#include "CVClock.h"
#include <Arduino.h>

CVClock g_Clock(2, 3, A0);
CVClock g_Clock2(4, 5, A1);
CVClock g_Clock3(6, 7, A2);
CVClock g_Clock4(8, 9, A3);
CVClock g_Clock5(10, 11, A4);

int g_DebugCntr;
uint32_t g_DebugMillis;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("CVClock start...");

  g_Clock.Begin();
  g_Clock2.Begin();
  g_Clock3.Begin();
  g_Clock4.Begin();
  g_Clock5.Begin();

  g_DebugCntr = 0;
  g_DebugMillis = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  // TODO alternate analog reads?
  g_Clock.Tick();
  g_Clock2.Tick();
  g_Clock3.Tick();
  g_Clock4.Tick();
  g_Clock5.Tick();
  //  delay(1);

  // debug...
  ++g_DebugCntr;
  if (1000 < g_DebugCntr) {
    uint32_t time = millis();
    g_Clock.debugOut(time - g_DebugMillis);
    g_Clock2.debugOut(time - g_DebugMillis);
    g_DebugMillis = time;
    g_DebugCntr = 0;
  }
}
