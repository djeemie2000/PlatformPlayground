#include <Arduino.h>
#include "delayline.h"
#include "fastdac.h"
#include "fastdigitalwrite.h"
#include "lowpassfilter.h"

#define DOSERIALDEBUG 1

struct NanoLooperApp
{
  static const int loopLength = 1700;// 1.7 kB
  static const int recordingLedPin = LED_BUILTIN; // 13 PB5
  static const int recordingButtonInPin = 2; //PD2
  static const int degradeInPin = 3;//PD3
  static const int antidegradeInPin = A3;//PC3
  static const int reverseInPin = A4;//PC4
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
    // button in
    pinMode(degradeInPin, INPUT_PULLUP);
    pinMode(antidegradeInPin, INPUT_PULLUP);
    pinMode(reverseInPin, INPUT_PULLUP);
    // recording led -> use builtin led for now
    pinMode(recordingLedPin, OUTPUT);

    prevRecordingBtn = 0;
    cvCntr = 0;
    delayusec = 0;
    resetCounter = 0;
    resetPeriod = 16000;
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
        resetPeriod = analogRead(resetPeriodInPin)<<4;// [0-32768[
        resetPeriod = max(128, resetPeriod);
        cvCntr = 0;
      }

      // recording led off
      fastDigitalWritePortB<5>(0);// pin13 PB5

      // read delay line
      //    write to fast dac out 
      dac.Write(delayLine.Read());

      // degrade sample
      if(!fastDigitalReadPortD<3>())
      {
        delayLine.Degrade();
      }
      
      if(!fastDigitalReadPortC<4>())
      {
        delayLine.AntiDegrade();
      }

      // advance delay line
      if(fastDigitalReadPortC<3>())
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
        resetCounter = 0;
#ifdef DOSERIALDEBUG
        Serial.println(resetPeriod);
#endif

      }
      ++resetCounter;

      //if(fastDigitalReadPortD<3>())
      {
        // delay ~ speed cv
        delayMicroseconds(delayusec);
      }
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

    analogRead(audioPinIn);
    delay(2);
    analogRead(audioPinIn);
    delay(2);

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
