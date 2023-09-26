#include <Arduino.h>

#include "bitbeatapp.h"

BitBeatApp app;

void PrintEEPROM(int offset, int length)
{
  Serial.println("EEPROM...");
  for(int off = offset; off<offset+length; ++off)
  {
    Serial.print(off);
    Serial.print(" 0x");
    Serial.print(EEPROM.read(off), HEX);
    Serial.print(' ');
    char c = EEPROM.read(off);
    Serial.println(c);
  }
  Serial.println("EEPROM");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("BitBeat v0.5...");

  app.Begin();

  Serial.println("Loading params...");
  app.LoadParams();

  PrintEEPROM(0, 90);

  Serial.println("Start running");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // test leds and buttons
  TestLedOut(app.m_LedOut, 1);
  TestDigitalOutBank(app.m_GateOut, 1);
  TestButtonInGet(app.m_ButtonIn, 4);
  TestButtonIn(app.m_ButtonIn, 4);
  TestButtonIn(app.m_FunctionButtonIn, 4);

  // all leds / gates off
  AllClear(app.m_GateOut);
  AllOff(app.m_LedOut);

  Serial.println("running");

  while(true)
  {
    app.Update();
  }
}
