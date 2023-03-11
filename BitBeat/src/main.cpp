#include <Arduino.h>

#include "bitbeatapp.h"

BitBeatApp app;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("BitBeat v0.2...");

  app.Begin();

  Serial.println("Loading params...");
  app.LoadParams(0);

  Serial.println("Start running");
}

void loop() {
  // put your main code here, to run repeatedly:
  //TestDigitalOutBank(app.m_GateOut, 2);
  //TestLedOut(app.m_LedOut, 1);
  //TestButtonInGet(app.m_ButtonIn, 1);
  //TestButtonIn(app.m_ButtonIn, 2);
  //TestButtonIn(app.m_FunctionButtonIn, 4);

  while(true)
  {
    app.Update();
  }
}
