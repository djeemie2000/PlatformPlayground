#pragma once
#include <Arduino.h>
//#include "MPR121TouchPad.h"
#include "Max7219Matrix.h"
// #include "DigitalIn.h"
// #include "DigitalInBank.h"
// #include "MultiTouchPad.h"
// #include "TTP8229TouchPad.h"
#include "AnalogIn.h"
//#include "TouchInBank.h"
#include "keypad.h"

struct DevBoard
{
  static const int LedPin = 2;
  static const int csPinLedMatrix = 5;
  // static const int irqPinTouchPadA = 15;
  static const int debugPin = 4;// TODO debugIn DIn


  // uart0 : usb + GPIO 1 & 3
  // uart1 : not connected
  // uart2 : GPIO 16 & 17
  HardwareSerial serialDebug;//uart 0
  bool debugIsActive;

  HardwareSerial serialMidi;// uart 2
  //TODO SDCard? -> SPI!
//  DigitalInBank IOXP1;
  AnalogIn Pot1;//tempo
  // TouchInBank touchIn;
  // MPR121TouchPad MPR121A;
  // TTP8229TouchPad_I2C TTP8299;
//  MultiTouchPad touchPad;
  KeyPad touchPad;

  Max7219Matrix ledMatrix;//(1,PA4);//cs pin

  DevBoard()
   : serialDebug(0)
   , debugIsActive(false)
   , serialMidi(2)
   //, IOXP1(34, 35)
  , Pot1()
  //, touchIn(T4, T5, T6, T7)
   //, MPR121A()
   , touchPad()
   , ledMatrix(1, csPinLedMatrix)//cs pin
   {}

   void Begin()
   {
     pinMode(LedPin, OUTPUT);// led builtin
     pinMode(debugPin, INPUT_PULLUP);

     serialDebug.begin(115200);
     serialMidi.begin(31250);

     //IOXP1.begin();
     Pot1.begin(15);//tempo pot
    //touchIn.begin(30, 45);//hysteresis min max


     // setup I2C for MPR121 MPR121A
     // Wire.begin() is called inside MPR121A.begin()
     //MPR121A.Begin(irqPinTouchPadA); //irq pin
     //touchPad.Add(&MPR121A);
    //  MPR121B.Begin(irqPinTouchPadB, MPR121TouchPad::DefaultTouchThreshold, MPR121TouchPad::DefaultReleaseThreshold, 0x5B); //irq pin
    //  touchPad.Add(&MPR121B);
     //TTP8299.Begin();
     //touchPad.Add(&TTP8299);
     //touchPad.Begin();
     //TODO autoDetect I2C : MPR121A memBank
     
     // GPIOs 34 to 39 are GPIs – input only pins. 
    touchPad.Begin(32,33,25,26,36,39,34,35);

      //SPI CS external
      ledMatrix.Configure();     
   }

   void update()
   {
     //TODO!!!!!!!!!!!!!!
    debugIsActive = true;//TODO!!!(0 == digitalRead(debugPin));
    //IOXP1.update();
    
    Pot1.Read();
    //  Pot2.Read();
    // update led matrix, touchpad here??
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
