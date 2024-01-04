#include <Arduino.h>
#include "nanolooperapp.h"


NanoLooperApp app;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Nanolooper v0.1");

  Serial.print("app begin...");
  app.Begin();
  Serial.println(" done");
}

void loop() {
  // put your main code here, to run repeatedly:

  //TestFastDacSlow(app.dac, 1, 0);
  //TestFactDacFastDescending(app.dac, 100);
//  return;

  app.update();
}

// put function definitions here:
