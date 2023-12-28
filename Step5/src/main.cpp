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

  AllClear<12>(board.stepSelectOut);
  AllClear<2>(board.gateOut);
}

void TestGateOutClockResetIn()
{
  // assumes gate out 1 / gate out 2 are connected to clock / reset in
  Serial.println("Set Gate 1 out");
  board.gateOut.Set(0);
  board.gateOut.Update();
  TestDigitalInGet(board.clockResetIn, 1);
  Serial.println("Clear Gate 1 out");
  board.gateOut.Clear(0);
  board.gateOut.Update();

  Serial.println("Set Gate 2 out");
  board.gateOut.Set(1);
  board.gateOut.Update();
  TestDigitalInGet(board.clockResetIn, 1);
  Serial.println("Clear Gate 2 out");
  board.gateOut.Clear(1);
  board.gateOut.Update();
}

void TestCVBus()
{
  // select bus 1 on output 1 vs select bus 2 on output 2
  Serial.println("Test CV bus...");
  Serial.println("Clear Select 1");
  Serial.println("Set Select 2");
  board.stepSelectOut.Clear(10);//left row
  board.stepSelectOut.Set(11);//right row
  // very slowly iterate through all leds/steps
  // so you can check the CV output at each step
  for(int step = 0; step<5; ++step)
  {
    int step2 = step + 5;
    Serial.print("Step ");
    Serial.print(step);
    Serial.print(" ");
    Serial.println(step2);

    AllClear<12>(board.stepSelectOut);
    board.stepSelectOut.Clear(10);//left row
    board.stepSelectOut.Set(11);//right row

    board.stepSelectOut.Set(step);
    board.stepSelectOut.Set(step2);

    board.stepSelectOut.Update();

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
  AllClear<12>(board.stepSelectOut);
  board.stepSelectOut.Set(0);
  board.stepSelectOut.Set(5);

  // very slowly iterate through all select combinations
  // so you can check the CV output at each combination
  Serial.println("Set Select 1");
  Serial.println("Clear Select 2");
  board.stepSelectOut.Set(10);
  board.stepSelectOut.Clear(11);
  board.stepSelectOut.Update();
  delay(10000);

  Serial.println("Clear Select 1");
  Serial.println("Set Select 2");
  board.stepSelectOut.Clear(10);
  board.stepSelectOut.Set(11);
  board.stepSelectOut.Update();
  delay(10000);

  Serial.println("Set Select 1");
  Serial.println("Set Select 2");
  board.stepSelectOut.Set(10);
  board.stepSelectOut.Set(11);
  board.stepSelectOut.Update();
  delay(10000);

  Serial.println("Clear Select 1");
  Serial.println("Clear Select 2");
  board.stepSelectOut.Clear(10);
  board.stepSelectOut.Clear(11);
  board.stepSelectOut.Update();
  delay(10000);
}

void loop() {
  // put your main code here, to run repeatedly:

  // quick test of all leds?
  AllClear(board.gateOut);
  AllClear(board.stepSelectOut);

  // while(true)
  // {
  //   TestDigitalOutBank<12>(board.stepSelectOut, 2);
  //   TestDigitalOutBank<2>(board.gateOut, 2);
  // }

//   while(true)
//   {
//     TestCVBus();
//   }

//   TestButtonIn(board.resetBtnIn, 5);
//   TestGateOutClockResetIn();
//   TestAnalogIn(board.lengthIn, 20);
//   return;

//   AllClear(board.stepOut);
//   AllClear(board.selectGateOut);

  while(true)
  {
    app.Update(board);
    delay(1);// mSec
  }

}

// put function definitions here:
