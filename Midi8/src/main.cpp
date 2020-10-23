#include <Arduino.h>
#include <SPI.h>

// #include "DigitalOutBank.h"
// #include "analogoutbank.h"
// #include "shiftoutbank.h"

#include "midi8ui.h"
#include "MidiNoteStack.h"

// DigitalOutBank gatesOut(4,5,6,7);
// AnalogOutBank cvOut(4);// 2x DAV => use CS pins 10,9
// ShiftOutBank ledsOut(8);//use CS pin 8

Midi8UI midi8UI;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(31250);//115200);
  Serial.println("Midi8 v0.1...");

  // gatesOut.begin();
  // cvOut.begin();
  // ledsOut.begin();
  midi8UI.begin();
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
  testAnalogOutBank(midi8UI.cvOut, 1);
  testDigitalOutBank(midi8UI.ledsOut,4);
  testDigitalOutBank(midi8UI.gatesOut, 4);
}
