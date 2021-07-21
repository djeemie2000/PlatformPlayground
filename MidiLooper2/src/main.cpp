#include <Arduino.h>
#include <Wire.h>
#include "ScanI2C.h"
#include "MPR121TouchPad.h"
#include "TestTouchPad.h"
#include "Max7219Matrix.h"
#include "TestDigitalOutMatrix.h"
#include "DigitalInBank.h"


//uint8_t testBuffer[16*1024];

struct DevBoard
{
  static const int LedPin = 2;
  static const int csPinLedMatrix = 5;
  static const int irqPinTouchPad = 15;
  static const int debugPin = 4;// TODO debugIn DIn


  // uart0 : usb + GPIO 1 & 3
  // uart1 : not connected
  // uart2 : GPIO 16 & 17
  HardwareSerial serialDebug;//uart 0
  bool debugIsActive;

  HardwareSerial serialMidi;// uart 2
  // TODO membank
  DigitalInBank IOXP1;
//  DigitalOutBank IOXP2;
//  DigitalOutBank IOXP3;
//  AnalogIn Pot1;
//  AnalogIn Pot2;
//  AnalogIn Pot3;
  MPR121TouchPad MPR121A;
  Max7219Matrix ledMatrix;//(1,PA4);//cs pin

  DevBoard()
   : serialDebug(0)
   , debugIsActive(false)
   , serialMidi(2)
   , IOXP1(34, 35)
  //  , IOXP2(PB3, PA15, PA12, PA11)
  //  , IOXP3(PB7, PB6, PB5, PB4)
  //  , Pot1()
  //  , Pot2()
  //  , Pot3()
   , MPR121A()
   , ledMatrix(1, csPinLedMatrix)//cs pin
   {}

   void Begin()
   {
     pinMode(LedPin, OUTPUT);// led builtin
     pinMode(debugPin, INPUT_PULLUP);

     serialDebug.begin(115200);
     serialMidi.begin(31250);

     IOXP1.begin();
    //  IOXP2.begin();
    //  IOXP3.begin();

    //  Pot1.begin(PA0);
    //  Pot2.begin(PA1);
    //  Pot3.begin(PA2);

     // setup I2C for MPR121 MPR121A
     // Wire.begin() is called inside MPR121A.begin()
     MPR121A.Begin(irqPinTouchPad); //irq pin
     //TODO autoDetect I2C : MPR121A memBank

      //SPI CS external
      ledMatrix.Configure();     
   }

   void update()
   {
     //TODO!!!!!!!!!!!!!!
    debugIsActive = (0 == digitalRead(debugPin));
    IOXP1.update();
    // IOXP2.update();
    // IOXP3.update();

    //  Pot1.Read();
    //  Pot2.Read();
    //  Pot3.Read();
    // update led matrix, touchpad here??
    MPR121A.Read();
    ledMatrix.WriteAll();
   }

   //TODO update() -> store in variable
   bool debugActive() const
   {
     return debugIsActive; 
   }

   void LedOn()
   {
     digitalWrite(LedPin, HIGH);
   }

   void LedOff()
   {
     digitalWrite(LedPin, LOW);
   }
};


DevBoard devBoard;

void setup() {
  // put your setup code here, to run once:
  devBoard.Begin();

  // Serial.begin(115200);// uart0 : usb + GPIO 1 & 3
  // pinMode(LedPin, OUTPUT);

  // Serial2.begin(31250);//midi serial uart2 : GPIO 16 & 17

  // //Wire.begin();
  // touchPad.Begin(irqPinTouchPad);// Wire.begin() is called inside MPR121A.begin()

  // ledMatrix.Configure();

  // memset(testBuffer, 0, sizeof(testBuffer));
}

void loop() {
  // put your main code here, to run repeatedly:

  ScanI2C(devBoard.serialDebug);
  
  const int waitMilliSeconds = 200; 

  devBoard.LedOn();  
  devBoard.serialDebug.println("LED is on");

  testDigitalOutMatrix(devBoard.ledMatrix, 1);

  for(int repeat = 0; repeat<waitMilliSeconds; ++repeat)
  {
    devBoard.update();
     delay(1);
  }
  PrintTouchPad(devBoard.MPR121A, devBoard.serialDebug);

  devBoard.LedOff();
  Serial.println("LED is off");

  testDigitalOutMatrix(devBoard.ledMatrix, 1);

  for(int repeat = 0; repeat<waitMilliSeconds; ++repeat)
  {
     devBoard.update();
     delay(1);
   }
  PrintTouchPad(devBoard.MPR121A, devBoard.serialDebug);

}
