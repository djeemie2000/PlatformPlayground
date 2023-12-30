#include <Arduino.h>
#include "delayline.h"
#include "fastdac.h"
#include "fastdigitalwrite.h"

#define DOSERIALDEBUG 1

struct NanoLooperApp
{
  static const int loopLength = 1700;//1536;// 1.5 kB
  static const int recordingLedPin = LED_BUILTIN; // 13 PB5
  static const int recordingButtonInPin = 2; //PD2
  static const int speedInPin = A0;
  static const int audioPinIn = A1;

  DelayLine<loopLength> delayLine;//assumes 328 2k memory
  FastDac dac;//assumes arduino nano
  int prevRecordingBtn;
  int recordingCounter;

  NanoLooperApp()
  {}

  void Begin()
  {
    delayLine.Begin();
    dac.Begin();
    // recording button in
    pinMode(recordingButtonInPin, INPUT_PULLUP);
    // recording led -> use builtin led for now
    pinMode(recordingLedPin, OUTPUT);

    prevRecordingBtn = 0;
    recordingCounter = 0;
  }

  void update()
  {
    // first read speed CV
    int delayusec = analogRead(speedInPin);// [0-4096[ milliseconds
    
    // then read audio in
    uint8_t audioValue = analogRead(audioPinIn)>>2;// [0,256[

    // then read record button in
    //    update recording state
    //    update recording led
    int recordingBtn = fastDigitalReadPortD<2>();//PD2
    if(!prevRecordingBtn && recordingBtn)
    {
#ifdef DOSERIALDEBUG
      Serial.println('R');
#endif
      recordingCounter = loopLength;
    }
    else if(0<recordingCounter)
    {
      --recordingCounter;
#ifdef DOSERIALDEBUG
      if(0==recordingCounter)
      {
        Serial.println('r');
      }
#endif
    }
    prevRecordingBtn = recordingBtn;
    fastDigitalWritePortB<5>(recordingCounter);// pin13 PB5

    // if recording 
    //    write delay line
    if(0<recordingCounter)
    {
      delayLine.Write(audioValue);
    }

    // read delay line
    //    write to fast dac out 
    dac.Write(delayLine.Read());

    // degrade sample
    //delayLine.Degrade();

    // advance delay line
    delayLine.Advance();

    // delay ~ speed cv
    delayMicroseconds(delayusec);
  }

};


NanoLooperApp app;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Nanolooper v0.1");

  Serial.print("app begin...");
  app.Begin();
  Serial.println(" done");
}

void loop() {
  // put your main code here, to run repeatedly:

  //TestFastDacSlow(app.dac, 1, 0);
  //TestFactDacFastDescending(app.dac, 100);
//  return;

  app.update();
}

// put function definitions here:
