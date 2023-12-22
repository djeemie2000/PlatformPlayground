#include <Arduino.h>

#include "step5board.h"
#include "step5app.h"

// put function declarations here:
Step5Board board;
Step5App app;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(1000);
  Serial.println("Step5 v0.1...");
  
  board.Begin();

  AllClear(board.stepOut);
  AllClear(board.selectGateOut);
}

void TestGateOutClockResetIn()
{
  // assumes gate out 1 / gate out 2 are connected to clock / reset in
  Serial.println("Set Gate 1 out");
  board.selectGateOut.Set(2);
  TestDigitalInGet(board.clockResetIn, 1);
  Serial.println("Clear Gate 1 out");
  board.selectGateOut.Clear(2);

  Serial.println("Set Gate 2 out");
  board.selectGateOut.Set(3);
  TestDigitalInGet(board.clockResetIn, 1);
  Serial.println("Clear Gate 2 out");
  board.selectGateOut.Clear(3);
}

void TestCVBus()
{
  // select bus 1 on output 1 vs select bus 2 on output 2
  Serial.println("Test CV bus...");
  Serial.println("Clear Select 1");
  Serial.println("Set Select 2");
  board.selectGateOut.Clear(0);//left row
  board.selectGateOut.Set(1);//right row
  // very slowly iterate through all leds/steps
  // so you can check the CV output at each step
  for(int step = 0; step<DigitalOutBank10::Size/2; ++step)
  {
    int step2 = step + DigitalOutBank10::Size/2;
    Serial.print("Step ");
    Serial.print(step);
    Serial.print(" ");
    Serial.println(step2);

    AllClear(board.stepOut);
    board.stepOut.Set(step);
    board.stepOut.Set(step2);

    delay(5);//?? needed ??
    board.bus1In.Read();
    board.bus2In.Read();
    Serial.print("Bus CV ");
    Serial.print(board.bus1In.Get());
    Serial.print(" ");
    Serial.println(board.bus2In.Get());

    delay(5000);
  }
}

void TestCVBus2()
{
  // select bus 1 on output 1 vs select bus 2 on output 2
  Serial.println("Test CV bus 2...");
  Serial.println("Set step 0 5");
  AllClear(board.stepOut);
  board.stepOut.Set(0);
  board.stepOut.Set(5);

  // very slowly iterate through all select combinations
  // so you can check the CV output at each combination
  Serial.println("Set Select 1");
  Serial.println("Clear Select 2");
  board.selectGateOut.Set(0);
  board.selectGateOut.Clear(1);
  delay(10000);

  Serial.println("Clear Select 1");
  Serial.println("Set Select 2");
  board.selectGateOut.Clear(0);
  board.selectGateOut.Set(1);
  delay(10000);

  Serial.println("Set Select 1");
  Serial.println("Set Select 2");
  board.selectGateOut.Set(0);
  board.selectGateOut.Set(1);
  delay(10000);

  Serial.println("Clear Select 1");
  Serial.println("Clear Select 2");
  board.selectGateOut.Clear(0);
  board.selectGateOut.Clear(1);
  delay(10000);
}

void loop() {
  // put your main code here, to run repeatedly:
  //int cntr = 0;

  // TODO quick test of all leds?
  AllClear(board.stepOut);
  AllClear(board.selectGateOut);

//   while(true)
//   {
// //    TestAnalogIn(board.lengthIn, 20);
//     TestCVBus();
//   }

//   TestDigitalOutBank(board.stepOut, 2);
//   TestDigitalOutBank(board.selectGateOut, 2);
//   TestButtonIn(board.resetBtnIn, 5);
// //  TestDigitalInGet(board.clockResetIn, 2);
//   TestGateOutClockResetIn();
//   TestAnalogIn(board.lengthIn, 20);
//   return;

//   AllClear(board.stepOut);
//   AllClear(board.selectGateOut);

  while(true)
  {
    app.Update(board);
    //delay(1);// mSec
  }

}

// put function definitions here:
