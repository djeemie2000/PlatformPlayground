#include <Arduino.h>
#include "nanolooperapp.h"

//#include "nanolooperspeedtest.h"
#include "testdac.h"
#include "testdevboard.h"

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
  Serial.println("Nanolooper v0.2");

  Serial.print("app begin...");
  app.Begin();
  Serial.println(" done");
}

void loop() {
  // put your main code here, to run repeatedly:

  TestGateOut(app);
  TestLedOut(app);

  //TestFastDacSlow(app.dac, 1, 0);
//  TestDacFastDescending(app.dac, 40);

  //TestAnalogInBank821(app.analogIn2, 20);
  //return;

    // DoSpeedTest();
    // return;

  //app.update();
}

// put function definitions here:
