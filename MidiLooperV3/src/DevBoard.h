#pragma once
#include <Arduino.h>
#include "MPR121TouchPad.h"
#include "Max7219Matrix.h"
#include "MultiTouchPad.h"
#include "AnalogIn.h"
#include "TouchInBank.h"
#include "sdstorage.h"

struct DevBoard
{
  static const int LedPin = 2;
  static const int csPinLedMatrix = 5;
  static const int csPinSDCard = 15;
  static const int irqPinTouchPadA = 34;
  static const int irqPinTouchPadB = 35;
  static const int debugPin = 4;// TODO debugIn DIn

  // uart0 : usb + GPIO 1 & 3
  // uart1 : not connected
  // uart2 : GPIO 16 & 17
  HardwareSerial serialDebug;//uart 0
  bool debugIsActive;

  HardwareSerial serialMidi;// uart 2
  AnalogIn Pot1;//tempo
  AnalogIn Pot2;//length
  TouchInBank touchIn;
  MPR121TouchPad MPR121A;
  MPR121TouchPad MPR121B;
  MultiTouchPad touchPad;

  Max7219Matrix ledMatrix;//(1,PA4);//cs pin
  
  SDStorage sdStorage;

  DevBoard()
   : serialDebug(0)
   , debugIsActive(false)
   , serialMidi(2)
   , Pot1()
   , Pot2()
   , touchIn(T4, T5, T6, T7, T8, T9)
   , MPR121A()
   , MPR121B()
   , ledMatrix(1, csPinLedMatrix)//cs pin
   , sdStorage()
   {}

   void Begin()
   {
     pinMode(LedPin, OUTPUT);// led builtin
     pinMode(debugPin, INPUT_PULLUP);

     serialDebug.begin(115200);
     serialMidi.begin(31250);

     //IOXP1.begin();
     Pot1.begin(36);//tempo pot
     Pot2.begin(39);//tempo pot
     touchIn.begin(30, 45);//hysteresis min max

     // setup I2C for MPR121 MPR121A
     // Wire.begin() is called inside MPR121A.begin()
     MPR121A.Begin(irqPinTouchPadA); //irq pin
     MPR121B.Begin(irqPinTouchPadB, MPR121TouchPad::DefaultTouchThreshold, MPR121TouchPad::DefaultReleaseThreshold, 0x5B); //irq pin
     touchPad.Add(&MPR121A);
     touchPad.Add(&MPR121B);
     touchPad.Begin();
     //TODO autoDetect I2C : MPR121A MPR121B
      
      //SPI CS external
      ledMatrix.Configure();

      sdStorage.Open(csPinSDCard);     
   }

   void update()
   {
     //TODO!!!!!!!!!!!!!!
    debugIsActive = true;//TODO!!!(0 == digitalRead(debugPin));
    //IOXP1.update();
    
    Pot1.Read();
    Pot2.Read();
    // update led matrix, touchpad here??
    touchPad.Read();
    touchIn.update();
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
