#include <Arduino.h>
#include "nanolooperapp.h"

//#include "nanolooperspeedtest.h"

NanoLooperApp app;

// void DoSpeedTest()
// {
//   FastDac dac1;
//   MCPDac dac2;

//   //setupFastAnalogRead();

//   dac1.Begin();
//   dac2.Begin();

//   while(true)
//   {
//     SpeedTestNoDac(A0);
//     SpeedTestFastDac(dac1, A0);
//     SpeedTestMCPDac(dac2, A0);    
//   }
// }

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

    // DoSpeedTest();
    // return;

  app.update();
}

// put function definitions here:
