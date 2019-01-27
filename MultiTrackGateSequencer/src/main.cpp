#include <mbed.h>
#include "GateIn.h"
#include "TrackPlayer.h"
#include "LogMidiHandler.h"
#include "CommonState.h"
#include "TrackController.h"
#include "SerialMidiHandler.h"
#include "PeriodicOut.h"
#include "Mpr121InBank.h"
#include "Max7219Matrix.h"
#include "BitWise.h"
#include "ScanI2C.h"
#include "MultiMidiHandler.h"
#include "GateMidiHandler.h"
#include "ClockInQuantizer.h"

#include "LogMidiHandler.h"
#include "MBedUnit.h"

int main() {

  // put your setup code here, to run once:
  DigitalOut ledOut(PC_13);//builtin led as gate indicator
  Serial debugSerial(PA_9, PA_10); // tx, rx
  debugSerial.baud(115200);
  Serial pcMidi(PA_2, PA_3); // tx, rx
  pcMidi.baud(31250);//midi baudrate

  // short pause before running
  wait_ms(2000);

  //testMain(debugSerial);//TODO debug

  // start running
  debugSerial.printf("\r\n-\r\n-\r\nMulti track gate sequencer...\r\n-\r\n-\r\n");
  debugSerial.printf("version 0.5\r\n");
  wait_ms(1000);

  //
  debugSerial.printf("Init led matrix...\r\n");
  SPI spiPort(PA_7, PA_6, PA_5, NC);
  Max7219Matrix ledMatrix(4, &spiPort, PA_4);//(SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_SS));//4 devices
  ledMatrix.Configure(false);
  //debugSerial.printf("Test led matrix...\r\n");
  //ledMatrix.Test();
  //TestExtended(ledMatrix, debugSerial);
  
  // common
  debugSerial.printf("Init common\r\n");
  SerialMidiHandler midiSerial(pcMidi);

  DigitalOut clockLed(PB_14);
  GateIn clockIn(PB_13);//external clock input
  PeriodicCounter internalClockCounter;
  ClockOutState internalClockState;
  DigitalOut internalClockOut(PB_12);
  AnalogIn internalClockSpeed(PA_1);
  
  DigitalOut midiNoteLearnLed(PB_15);
  DigitalOut midiChannelLearnLed(PA_8);
  DigitalOut gateLengthLearnLed(PB_8);
  ButtonIn toggleModeBtn(PB_0,5);//debounce 5msec
  ToggleNState learnMode(4);
  //ToggleInOut playStepModeBtn(PB_8, PB_14);// play/Step mode toggle btn, with indicator led
  //GateIn resetAdvanceBtn(PB_9);// reset/advance btn
  GateState resetStepState;
  AnalogIn learnValuePot(PA_0);
  CommonState commonState;
  
  I2C i2c(PB_11, PB_10);
  ScanI2C(i2c, debugSerial);
  // set, mute, clear btn, unused, 8x track button 
  Mpr121InBank touchPad(&i2c, PB_1);

  // multiple tracks
  debugSerial.printf("Init tracks\r\n");
  const int NumTracks = 8;
  const int PatternLength = 32;
  //default note, pattern, channel, gate outputs, midi handlers
  const uint8_t midiNotes[] = {0x23, 0x24, 0x25, 0x2B, 0x2D, 0x30, 0x32, 0x34};
  const uint32_t patterns[] = {0x11111111, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xAA88AA88};
  const uint8_t MidiChannel = 0x03;//channel 4
  const PinName outPins[] = {PA_11, PA_12, PA_15, PB_3, PB_4, PB_5, PB_6, PB_7 };
  DigitalOut* midiOutputs[NumTracks];
  MultiMidiHandler multis[NumTracks];

  TrackController* tracks[NumTracks];
  for(int idx = 0; idx<NumTracks; ++idx)
  {
    // yes, these are memory leaks (if we would ever destruct)
    midiOutputs[idx] = new DigitalOut(outPins[idx]);
    multis[idx].AddHandler(&midiSerial);
    multis[idx].AddHandler(new GateMidiHandler(*midiOutputs[idx]));
    //multis[idx].AddHandler(new LogMidiHandler(debugSerial, idx));//debug
    tracks[idx] = new TrackController(multis[idx], midiNotes[idx], MidiChannel, idx, ledMatrix);
    tracks[idx]->SetPattern(patterns[idx], PatternLength);
  }

  // 
  Timer timer;
  int counter = 0;
  const int fakeClockPeriod = 220;
  ClockInState clockInState(fakeClockPeriod);

  wait_ms(500);
  debugSerial.printf("start processing\r\n");
  while(1) {
    // put your main code here, to run repeatedly:
      timer.reset();
      timer.start();

      for(int repeat1 = 0; repeat1<4; ++repeat1)
      {
        for(int repeat = 0; repeat<fakeClockPeriod; ++repeat)
        {
          clockIn.Read();
          //playStepModeBtn.Read();
          //resetAdvanceBtn.Read();
          toggleModeBtn.Read();
          touchPad.Read();
          learnMode.Tick(toggleModeBtn.Get());
          // use pad 8 for reset
          resetStepState.Tick(touchPad.Get(11));

          int internalClockSp = 40+internalClockSpeed.read()*1024;
          internalClockCounter.SetPeriod(internalClockSp);
          internalClockCounter.Tick();
          internalClockState.Tick(internalClockCounter.Cntr(), internalClockCounter.Period());

          commonState.setPressed = touchPad.Get(8);
          commonState.clearPressed = touchPad.Get(9);
          commonState.mutePressed = touchPad.Get(10);
          commonState.learnMode = learnMode.Get();
          commonState.learnValue = learnValuePot.read();
          commonState.playMode = true;//playStepModeBtn.Get();
          commonState.resetStepPressed = resetStepState.Get();
          
          // step mode => do not update period!
          //clockInState.Tick(internalClockState.IsRising(), commonState.playMode);
          clockInState.Tick(clockIn.IsRising(), commonState.playMode);
          commonState.clockCntr = clockInState.Cntr();
          commonState.clockPeriod = clockInState.Period();

          // clock
          // if play mode, use (fake) clock
          // if step mode, use reset/advance btn
          if(commonState.playMode)
          {
            commonState.clockIsRising = clockIn.IsRising();
            commonState.clockIsFalling = clockIn.IsFalling();
            commonState.clockOn = clockIn.Get();
          }
          // else
          // {
          //   commonState.clockIsRising = resetAdvanceBtn.IsRising();
          //   commonState.clockIsFalling = resetAdvanceBtn.IsFalling();
          //   commonState.clockOn = resetAdvanceBtn.Get();
          // }

          for(int idx = 0; idx<NumTracks; ++idx)
          {
            int allTrackBtn = 0;
            tracks[idx]->Tick(commonState, touchPad.Get(idx), allTrackBtn);
          }          

          // update display takes some time => update alternating rows
          ledMatrix.Write(repeat%NumTracks);

          // clock
          clockLed = internalClockState.Get();
          ledOut = 1-internalClockState.Get();//inverted
          internalClockOut = internalClockState.Get();

          midiNoteLearnLed = (learnMode.Get()==1)?1:0;
          midiChannelLearnLed = (learnMode.Get()==2)?1:0;
          gateLengthLearnLed = (learnMode.Get()==3)?1:0;
          // 
          wait_ms(1);
        }
      }
      
      timer.stop();
      debugSerial.printf("mute %d set %d clear %d learn %d play %d int %d ext %d val %f %f", 
                  commonState.mutePressed?1:0, 
                  commonState.setPressed?1:0, 
                  commonState.clearPressed?1:0, 
                  commonState.learnMode,
                  commonState.playMode?1:0,
                  internalClockCounter.Period(),
                  clockInState.Period(),
                  commonState.learnValue,
                  internalClockSpeed.read());
      debugSerial.printf("touchpad %d%d%d%d%d%d%d%d%d%d%d%d\r\n", 
                  touchPad.Get(0), 
                  touchPad.Get(1), 
                  touchPad.Get(2), 
                  touchPad.Get(3), 
                  touchPad.Get(4), 
                  touchPad.Get(5), 
                  touchPad.Get(6), 
                  touchPad.Get(7), 
                  touchPad.Get(8), 
                  touchPad.Get(9), 
                  touchPad.Get(10), 
                  touchPad.Get(11));
      debugSerial.printf("%d : %d ms\r\n", counter++, timer.read_ms());      
  }
}