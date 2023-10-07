#include <Arduino.h>
#include "orderedlist.h"
#include "testorderedlist.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("MidiFreeLooper v0.1...");
}

void DoTests()
{
  TestListAddInOrder();
  delay(1000);

  TestListAddResetHeadAdd();
  delay(1000);
  
  TestListRead();
  delay(1000);

  TestListReadWrite();
  delay(1000);
  
  delay(20000);

}

// TODO poc Midi buffer byte read -> first byte thru, then parse note events + midi clock/start/stop
// TODO poc leds out (reset pulse, recording/armed/..., play/mute, empty yes/no)
// TODO poc fixed length e.g. 24ppq*8, fixed recording on, fixed play 
// TODO poc play/mute
// TODO poc clear
// TODO poc recording off/armed/on
// TODO poc reset (?)


void loop() {
  // put your main code here, to run repeatedly:

  DoTests();
}


