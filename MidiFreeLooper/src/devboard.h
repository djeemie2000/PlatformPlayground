#pragma once
#include <Arduino.h>
#include "DigitalInBank.h"
#include "DigitalOutBank.h"
#include "AnalogIn.h"

// blue pill devboard Hardware abstraction
struct DevBoard
{
  // RX1 TX1 PA10 PA9
  // RX2 TX2 PA3 PA2
  // RX3 TX3 PB11 PB10
  HardwareSerial serialDebug;//(PA10, PA9);
  static const int debugPin = PA8;// TODO debugIn DIn
  HardwareSerial serialMidi;//(PB11, PB10);
  DigitalInBank IOXP1;
  DigitalOutBank IOXP2;
  DigitalOutBank IOXP3;
  AnalogIn Pot1;
  AnalogIn Pot2;
  AnalogIn Pot3;

  DevBoard()
   : serialDebug(PA10, PA9)
   , serialMidi(PB11, PB10)
   , IOXP1(PB15, PB14, PB13, PB12)
   , IOXP2(PB3, PA15, PA12, PA11)
   , IOXP3(PB7, PB6, PB5, PB4)
   , Pot1()
   , Pot2()
   , Pot3()
   {}

   void Begin()
   {
     pinMode(PC13, OUTPUT);// led builtin
     pinMode(debugPin, INPUT_PULLUP);

     serialDebug.begin(115200);
     serialMidi.begin(31250);

     IOXP1.begin();
     IOXP2.begin();
     IOXP3.begin();

     Pot1.begin(PA0);
     Pot2.begin(PA1);
     Pot3.begin(PA2);

   }

   void update()
   {
     //TODO!!!!!!!!!!!!!!
    IOXP1.update();
    IOXP2.update();
    IOXP3.update();

     Pot1.Read();
     Pot2.Read();
     Pot3.Read();
   }

   //TODO update() -> store in variable
   bool debugActive() const
   {
     return 0 == digitalRead(debugPin);
   }

   void LedOn()
   {
     digitalWrite(PC13, LOW);
   }

   void LedOff()
   {
     digitalWrite(PC13, HIGH);
   }
};

//DevBoard devBoard;
