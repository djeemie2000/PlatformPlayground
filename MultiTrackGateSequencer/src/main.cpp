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

#include "MBedUnit.h"

int main() {

  // put your setup code here, to run once:
  DigitalOut ledOut(PC_13);//builtin led as gate indicator
  Serial pc2(PA_9, PA_10); // tx, rx
  pc2.baud(115200);
  Serial pcMidi(PA_2, PA_3); // tx, rx
  pcMidi.baud(31250);//midi baudrate

  // short pause before running
  wait_ms(2000);

  //testMain(pc2);//TODO debug

  // start running
  pc2.printf("\r\n-\r\n-\r\nMulti track gate sequencer...\r\n-\r\n-\r\n");
  pc2.printf("version 0.5\r\n");
  wait_ms(1000);

  //
  pc2.printf("Init led matrix...\r\n");
  SPI spiPort(PA_7, PA_6, PA_5, NC);
  Max7219Matrix ledMatrix(4, &spiPort, PA_4);//(SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_SS));//4 devices
  ledMatrix.Configure(false);
  //pc2.printf("Test led matrix...\r\n");
  //ledMatrix.Test();
  //TestExtended(ledMatrix, pc2);
  
  // common
  pc2.printf("Init common\r\n");
  SerialMidiHandler midiSerial(pcMidi);

  DigitalOut clockLed(PB_14);
  GateIn clockIn(PA_8);//external clock input
  
  DigitalOut midiNoteLearnLed(PC_14);
  DigitalOut midiChannelLearnLed(PC_15);
  DigitalOut gateLengthLearnLed(PA_1);
  ButtonIn toggleModeBtn(PB_15,5);//debounce 5msec
  ToggleNState learnMode(4);
  //ToggleInOut playStepModeBtn(PB_8, PB_14);// play/Step mode toggle btn, with indicator led
  //GateIn resetAdvanceBtn(PB_9);// reset/advance btn
  GateState resetStepState;
  AnalogIn learnValuePot(PA_0);
  CommonState commonState;
  
  I2C i2c(PB_11, PB_10);
  ScanI2C(i2c, pc2);
  // set, mute, clear btn, unused, 8x track button 
  Mpr121InBank touchPad(&i2c, PB_1);

  // multiple tracks
  pc2.printf("Init tracks\r\n");
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
    tracks[idx] = new TrackController(multis[idx], midiNotes[idx], MidiChannel, idx, ledMatrix);
    tracks[idx]->SetPattern(patterns[idx], PatternLength);
  }

  // 
  Timer timer;
  int counter = 0;
  const int fakeClockPeriod = 220;
  GateState fakeClock;
  ClockInState clockInState(fakeClockPeriod);

  wait_ms(500);
  pc2.printf("start processing\r\n");
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
          resetStepState.Tick(touchPad.Get(8));
          
          //fake clock
          fakeClock.Tick(repeat<fakeClockPeriod/2?1:0);

          commonState.setPressed = touchPad.Get(10);
          commonState.clearPressed = touchPad.Get(11);
          commonState.mutePressed = touchPad.Get(9);
          commonState.learnMode = learnMode.Get();
          commonState.learnValue = learnValuePot.read();
          commonState.playMode = true;//playStepModeBtn.Get();
          commonState.resetStepPressed = resetStepState.IsRising();
          
          // step mode => do not update period!
          clockInState.Tick(fakeClock.IsRising(), commonState.playMode);
          //TODO clockInState.Tick(clockIn.IsRising(), commonState.playMode);
          commonState.clockCntr = clockInState.Cntr();
          commonState.clockPeriod = clockInState.Period();

          // fake clock
          // if play mode, use (fake) clock
          // if step mode, use reset/advance btn
          if(commonState.playMode)
          {
            commonState.clockIsRising = fakeClock.IsRising();
            commonState.clockIsFalling = fakeClock.IsFalling();
            commonState.clockOn = fakeClock.Get();
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
            tracks[idx]->Tick(commonState, touchPad.Get(7-idx), allTrackBtn);
          }          

          // update display takes some time => update alternating rows
          ledMatrix.Write(repeat%NumTracks);

          // fake clock
          clockLed = fakeClock.Get();
          ledOut = 1-fakeClock.Get();//inverted
          //ledOut = clockIn.Get()? 0:1;//inverted
          midiNoteLearnLed = (learnMode.Get()==1)?1:0;
          midiChannelLearnLed = (learnMode.Get()==2)?1:0;
          gateLengthLearnLed = (learnMode.Get()==3)?1:0;
          // 
          wait_ms(1);
        }
      }
      
      timer.stop();
      pc2.printf("mute %d set %d clear %d learn %d play %d ", 
                  commonState.mutePressed?1:0, 
                  commonState.setPressed?1:0, 
                  commonState.clearPressed?1:0, 
                  commonState.learnMode,
                  commonState.playMode?1:0);
      pc2.printf("touchpad %d%d%d%d%d%d%d%d%d%d%d%d\r\n", 
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
      pc2.printf("%d : %d ms\r\n", counter++, timer.read_ms());      
  }
}