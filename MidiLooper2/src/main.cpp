#include <Arduino.h>
#include <Wire.h>
#include "ScanI2C.h"
#include "MPR121TouchPad.h"
#include "TestTouchPad.h"
#include "Max7219Matrix.h"
#include "TestDigitalOutMatrix.h"


const int LedPin = 2;
// MPR121TouchPad touchPad;
const int csPin = 5;
Max7219Matrix ledMatrix(1, csPin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LedPin, OUTPUT);

  //touchPad.Begin(15);// is called inside MPR121A.begin()

  ledMatrix.Configure();
}

void loop() {
  // put your main code here, to run repeatedly:

//  ScanI2C(Serial);
  
  //const int waitMilliSeconds = 400; 
  
  digitalWrite(LedPin, HIGH);
  Serial.println("LED is on");

  testDigitalOutMatrix(ledMatrix, 1);

  // for(int repeat = 0; repeat<waitMilliSeconds; ++repeat)
  // {
  //   touchPad.Read();
  //   delay(1);
  // }
  // PrintTouchPad(touchPad, Serial);

  digitalWrite(LedPin, LOW);
  Serial.println("LED is off");

  testDigitalOutMatrix(ledMatrix, 1);

  // for(int repeat = 0; repeat<waitMilliSeconds; ++repeat)
  // {
  //   touchPad.Read();
  //   delay(1);
  // }
  //PrintTouchPad(touchPad, Serial);

}
