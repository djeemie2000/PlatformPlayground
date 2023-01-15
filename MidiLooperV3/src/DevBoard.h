#pragma once
#include <Arduino.h>
#include "MPR121TouchPad.h"
#include "Max7219Matrix.h"
#include "MultiTouchPad.h"
#include "AnalogIn.h"
#include "TouchInBank.h"
#include "sdstorage.h"
#include "mcp23017digitaliobank.h"

struct DevBoard
{
  static const int LedPin = 2;
  static const int csPinLedMatrix = 5;
  static const int csPinSDCard = 15;
  static const int debugPin = 4;// TODO debugIn DIn

  // uart0 : usb + GPIO 1 & 3
  // uart1 : not connected
  // uart2 : GPIO 16 & 17
  HardwareSerial serialDebug;//uart 0
  bool debugIsActive;

  HardwareSerial serialMidi;// uart 2

  Max7219Matrix ledMatrix;//(1,PA4);//cs pin
  
  MCP23017DigitalIOBank ioBank;

  SDStorage sdStorage;

  DevBoard()
   : serialDebug(0)
   , debugIsActive(false)
   , serialMidi(2)
   , ledMatrix(1, csPinLedMatrix)//cs pin
   , sdStorage()
   {}

   void Begin()
   {
     pinMode(LedPin, OUTPUT);// led builtin
     pinMode(debugPin, INPUT_PULLUP);

     serialDebug.begin(115200);
     serialMidi.begin(31250);
      
      //SPI CS external
      ledMatrix.Configure();

      ioBank.Begin(0x20);
      //TODO setup all GPIO as input pullup
      // for now, for testing, set all as outputs
      for(int pin = 0; pin<MCP23017DigitalIOBank::Capacity; ++pin)
      {
        ioBank.SetPinMode(pin, INPUT_PULLUP);
      }
      ioBank.WritePinModes();

      sdStorage.Open(csPinSDCard);     
   }

   void update()
   {
     //TODO!!!!!!!!!!!!!!
    debugIsActive = true;//TODO!!!(0 == digitalRead(debugPin));
    //IOXP1.update();
    
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
