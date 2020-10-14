#include <Arduino.h>
#include <SPI.h>

#include "DigitalOutBank.h"
#include "analogoutbank.h"
#include "shiftoutbank.h"

DigitalOutBank gatesOut(4,5,6,7);
AnalogOutBank cvOut(4);// 2x DAV => use CS pins 10,9
ShiftOutBank ledsOut(8);//use CS pin 8

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Midi8 v0.1...");

  gatesOut.begin();
  cvOut.begin();
  ledsOut.begin();
}

void ledPattern(uint8_t pattern)
{
  for(int idx = 0; idx<8; ++idx)
  {
    uint8_t curr = (pattern>>idx) & 0x01;
    digitalWrite(LED_BUILTIN, curr);
    delay(200);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //ledPattern(0xA5);
  testAnalogOutBank(cvOut, 1);
  testDigitalOutBank(ledsOut,4);
}
