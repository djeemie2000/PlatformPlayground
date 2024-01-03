#pragma once
#include <Arduino.h>

#include "delayline.h"
#include "fastdac.h"
#include "fastdigitalwrite.h"
#include "lowpassfilter.h"
#include "pseudorandom.h"
#include "bitcrush.h"

struct NanoLooperApp
{
  static const int loopLength = 1024+512;// 1.7 kB
  // output pins
  static const int recordingLedPin = LED_BUILTIN; // 13 PB5
  static const int randomOutPin = 12;//PB4
  // button in pins
  static const int recordingButtonInPin = 2; //PD2
  static const int degradeInPin = 3;//PD3
  static const int saturateInPin = A3;//PC3
  static const int reverseInPin = A4;//PC4
  // CV pins
  static const int speedInPin = A0;//PC0
  static const int audioPinIn = A1;//PC1
  static const int resetPeriodInPin = A2;//PC2
  static const int bitcrushInPin = A6;

  DelayLine<loopLength> delayLine;//assumes 328 2k memory
  FastDac dac;//assumes arduino nano
  PseudoRandom pseudoRandom;
  int randomState;

  int prevRecordingBtn;
  int cvCntr;

  int delayusec;

  int resetCounter;
  int resetPeriod;

  int bitcrush;

  NanoLooperApp()
  {}

  void Begin()
  {
    delayLine.Begin();
    dac.Begin();
    // recording button in
    pinMode(recordingButtonInPin, INPUT_PULLUP);
    // button in
    pinMode(degradeInPin, INPUT_PULLUP);
    pinMode(saturateInPin, INPUT_PULLUP);
    pinMode(reverseInPin, INPUT_PULLUP);
    // recording led -> use builtin led for now
    pinMode(recordingLedPin, OUTPUT);
    // random out pin
    pinMode(randomOutPin, OUTPUT);

    prevRecordingBtn = 0;
    cvCntr = 0;
    delayusec = 0;
    resetCounter = 0;
    resetPeriod = 16000;
    randomState = 1;
    bitcrush =0;
  }

  bool RecordingBtnClicked()
  {
    int recordingBtn = fastDigitalReadPortD<2>();//PD2
    bool clicked = !prevRecordingBtn && recordingBtn;
    prevRecordingBtn = recordingBtn;
    return clicked;
  }

  void update()
  {
    // first read record button in
    if(RecordingBtnClicked())
    {
      DoRecording();
    }
    else
    {
      DoPlayback(); 
    }
  }

  void DoPlayback()
  {
      // first read CV
      // alternate between speed CV and reset period CV
      if(0 == cvCntr)
      {
        delayusec = analogRead(speedInPin)>>1;// [0-512[ microseconds
        cvCntr = 1;
      }
      else if(1==cvCntr)
      {
        resetPeriod = analogRead(resetPeriodInPin)<<4;// [0-32768/2[
        resetPeriod = max(128, resetPeriod);
        cvCntr = 2;
      }
      else
      {
        bitcrush = analogRead(bitcrushInPin)>>7;//[0,7]
        cvCntr = 0;
      }

      // recording led off
      fastDigitalWritePortB<5>(0);// pin13 PB5

      // update random
      uint32_t random = pseudoRandom.Rand();
      //Serial.println(random);
      // TODO compare with threshold ~ CV/pot
      if((random & 0xFFFF)<128)
      {
        // toggle state
        randomState = 1-randomState;
      }
      fastDigitalWritePortB<4>(randomState);

      // degrade sample
      if(!fastDigitalReadPortD<3>())
      {
        delayLine.Degrade();
      }
      
      if(!fastDigitalReadPortC<3>())
      {
        delayLine.Saturate();
      }


      // read delay line
      //    write to fast dac out 
      uint8_t audioOut = delayLine.Read();

      // bitcrush ~ CV [0, 7]
      if(bitcrush>5)
      {
        audioOut = BitCrush2(audioOut, bitcrush-5);
      }
      else
      {
        audioOut = BitCrush1(audioOut, bitcrush);
      }
      dac.Write(audioOut);


      // advance/reverse delay line
      if(fastDigitalReadPortC<4>())
      {
        delayLine.Advance();
      }
      else
      {
        delayLine.Reverse();
      }

      // periodic reset 
      if(resetPeriod<resetCounter)
      {
        delayLine.Reset();
//        pseudoRandom.Reset();//???
        resetCounter = 0;
#ifdef DOSERIALDEBUG
        Serial.println(resetPeriod);
#endif
      }
      ++resetCounter;

      // delay ~ speed cv
      delayMicroseconds(delayusec);
  }

  void DoRecording()
  {
#ifdef DOSERIALDEBUG
      Serial.println('R');
#endif

    // always sample at highest freq

    // reset delay line
    delayLine.Reset();
    pseudoRandom.Reset();
    resetCounter = 0;

    // analogRead(audioPinIn);
    // delay(2);
    // analogRead(audioPinIn);
    // delay(2);

    int cntr = 0; 
    while(cntr<loopLength)
    {
       // read audio in
      uint8_t audioValue = analogRead(audioPinIn)>>2;// [0,256[

        // recording led on
      fastDigitalWritePortB<5>(1);// pin13 PB5

      //  write delay line
      delayLine.Write(audioValue);

      // read delay line
      //    write to fast dac out 
      dac.Write(0x7F);//delayLine.Read());

      // advance delay line
      delayLine.Advance();

      // delay ~ speed cv
      //delayMicroseconds(delayusec);

      ++cntr;
    }

#ifdef DOSERIALDEBUG
        Serial.println('r');
#endif
  }

};
