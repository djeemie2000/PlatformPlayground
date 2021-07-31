#pragma once
#include <Arduino.h>
#include "MPR121TouchPad.h"
#include "Max7219Matrix.h"
#include "DigitalIn.h"
#include "DigitalInBank.h"
#include "MultiTouchPad.h"
#include "TTP8229TouchPad.h"

struct DevBoard
{
  static const int LedPin = 2;
  static const int csPinLedMatrix = 5;
  static const int irqPinTouchPadA = 15;
  static const int irqPinTouchPadB = 4;
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
  //MPR121TouchPad MPR121B;
  TTP8229TouchPad_I2C TTP8299;

  MultiTouchPad touchPad;
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
//   , MPR121B()
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
     MPR121A.Begin(irqPinTouchPadA); //irq pin
     touchPad.Add(&MPR121A);
    //  MPR121B.Begin(irqPinTouchPadB, MPR121TouchPad::DefaultTouchThreshold, MPR121TouchPad::DefaultReleaseThreshold, 0x5B); //irq pin
    //  touchPad.Add(&MPR121B);
     TTP8299.Begin();
     touchPad.Add(&TTP8299);
     touchPad.Begin();
     //TODO autoDetect I2C : MPR121A memBank

      //SPI CS external
      ledMatrix.Configure();     
   }

   void update()
   {
     //TODO!!!!!!!!!!!!!!
    debugIsActive = true;//TODO!!!(0 == digitalRead(debugPin));
    IOXP1.update();
    // IOXP2.update();
    // IOXP3.update();

    //  Pot1.Read();
    //  Pot2.Read();
    //  Pot3.Read();
    // update led matrix, touchpad here??
    //MPR121A.Read();
    //MPR121B.Read();
    touchPad.Read();
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
