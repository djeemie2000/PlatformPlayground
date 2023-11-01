#include <Arduino.h>

#include "step5board.h"

// put function declarations here:
Step5Board board;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(1000);
  Serial.println("Step5 v0.1...");
  
  board.Begin();

  AllClear(board.selectGateOut);
}

void TestLeftRight(int repeats)
{   
    Serial.println("test left right");

    board.stepOut.Set(0);
    board.stepOut.Clear(1);
  //    delay(1);
    TestAnalogIn(board.bus1In, board.bus2In, repeats);

    board.stepOut.Clear(0);
    board.stepOut.Set(1);
    TestAnalogIn(board.bus1In, board.bus2In, repeats);
    
    board.stepOut.Set(5);
    board.stepOut.Clear(6);
    TestAnalogIn(board.bus1In, board.bus2In, repeats);
    
  //  delay(1);
    board.stepOut.Clear(5);
    board.stepOut.Set(6);
    TestAnalogIn(board.bus1In, board.bus2In, repeats);
}

void TestGateOut()
{
  Serial.println("test gate out");
  board.selectGateOut.Set(2);
  delay(500);
  board.selectGateOut.Clear(2);
  delay(500);
  board.selectGateOut.Set(3);
  delay(500);
  board.selectGateOut.Clear(3);
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  int cntr = 0;

  AllClear(board.stepOut);
  AllClear(board.selectGateOut);

  // for(int idx = 0; idx<2; ++idx)
  // {
  //     TestGateOut();
  // }

  while(true)
  {
    Serial.print("--- cntr ");
    Serial.println(cntr++);

    board.stepOut.Set(0);
    board.stepOut.Set(6);
    TestAnalogIn(board.lengthIn, board.bus1In, board.bus2In, 10);
    continue;

//    TestDigitalOutBank(board.stepOut, 1);
    //TestButtonIn(board.resetBtnIn, 2);
//    TestAnalogIn(board.lengthIn, 10);

    Serial.println("select set 0 set 1");
    board.selectGateOut.Set(0);
    board.selectGateOut.Set(1);
    TestLeftRight(4);
   
    Serial.println("select set 0 clear 1");
    board.selectGateOut.Set(0);
    board.selectGateOut.Clear(1);
    TestLeftRight(4);

    Serial.println("select clear 0 set 1");
    board.selectGateOut.Clear(0);
    board.selectGateOut.Set(1);
    TestLeftRight(4);

    Serial.println("select clear 0 clear 1");
    board.selectGateOut.Clear(0);
    board.selectGateOut.Clear(1);
    TestLeftRight(4);


    // Serial.println("test left select 01 clear");
    // board.selectGateOut.Clear(0);
    // board.selectGateOut.Clear(1);
    // board.stepOut.Set(0);
    // TestAnalogIn(board.bus1In, 10);
    // board.stepOut.Clear(0);

    // Serial.println("test left select 01 set");
    // board.selectGateOut.Set(0);
    // board.selectGateOut.Set(1);
    // board.stepOut.Set(0);
    // TestAnalogIn(board.bus1In, 10);
    // board.stepOut.Clear(0);


    // Serial.println("test right select 01 clear");
    // board.selectGateOut.Clear(0);
    // board.selectGateOut.Clear(1);
    // board.stepOut.Set(5);
    // TestAnalogIn(board.bus2In, 10);
    // board.stepOut.Clear(5);

    // Serial.println("test right select 01 set");
    // board.selectGateOut.Set(0);
    // board.selectGateOut.Set(1);
    // board.stepOut.Set(5);
    // TestAnalogIn(board.bus2In, 10);
    // board.stepOut.Clear(5);

    
    //TestAnalogIn(board.bus2In, 10);

    
    delay(500);
  }

}

// put function definitions here:
