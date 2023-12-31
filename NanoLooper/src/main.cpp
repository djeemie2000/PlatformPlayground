#include <Arduino.h>
#include "delayline.h"
#include "fastdac.h"
#include "fastdigitalwrite.h"

#define DOSERIALDEBUG 1

struct NanoLooperApp
{
  static const int loopLength = 1762;// 1.7 kB
  static const int recordingLedPin = LED_BUILTIN; // 13 PB5
  static const int recordingButtonInPin = 2; //PD2
  static const int speedInPin = A0;
  static const int audioPinIn = A1;
  static const int resetPeriodInPin = A2;

  DelayLine<loopLength> delayLine;//assumes 328 2k memory
  FastDac dac;//assumes arduino nano
  int prevRecordingBtn;
  int cvCntr;

  int delayusec;

  int resetCounter;
  int resetPeriod;

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
    cvCntr = 0;
    delayusec = 0;
    resetCounter = 0;
    resetPeriod = 0xFFFF;
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
      else
      {
        resetPeriod = analogRead(resetPeriodInPin)<<5;// [0-32768[
        resetPeriod = max(64, resetPeriod);
        cvCntr = 0;
      }

      // recording led off
      fastDigitalWritePortB<5>(0);// pin13 PB5

      // read delay line
      //    write to fast dac out 
      dac.Write(delayLine.Read());

      // degrade sample
      //delayLine.Degrade();

      // advance delay line
      delayLine.Advance();

      // periodic reset 
      if(resetPeriod<resetCounter)
      {
        delayLine.Reset();
        resetCounter = 0;
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

    // first read speed CV (once)
    //int delayusec = 0;// analogRead(speedInPin);// [0-4096[ milliseconds
    // ?? always sample at highest freq?

    // reset delay line
    delayLine.Reset();
    resetCounter = 0;

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
