#include <mbed.h>
#include "GateIn.h"
#include "GateTrack.h"
#include "LogMidiHandler.h"
#include "CommonState.h"
#include "TrackController.h"
#include "max7219.h"
#include "SerialMidiHandler.h"
#include "MultiMidiHandler.h"
#include "PeriodicOut.h"

int main() {

//   bluepill::Pin<1> pin1;
// //  bluepill::Pin<2> pin2;
//   bluepill::Pin<3> pin3;
//   PinName tmp = pin1;
//   tmp = bluepill::Pin<2>::pinname();
//   tmp = pin3;
//   tmp = bluepill::Pin<2>::name;

  // put your setup code here, to run once:
  DigitalOut ledOut(PC_13);//builtin led as gate indicator
  Serial pc2(PA_9, PA_10); // tx, rx
  pc2.baud(115200);
  Serial pcMidi(PA_2, PA_3); // tx, rx
  pcMidi.baud(31250);//midi baudrate

  // short pause before running
  wait_ms(2000);

  // start running
  pc2.printf("\r\n-\r\n-\r\nMulti track gate sequencer...\r\n-\r\n-\r\n");
  pc2.printf("version 0.2\r\n");
  wait_ms(1000);

  //
  pc2.printf("Init led matrix...\r\n");
  max7219_configuration_t cfg = {
         .device_number = 1,
         .decode_mode = 0,
         .intensity = Max7219::MAX7219_INTENSITY_1,
         .scan_limit = Max7219::MAX7219_SCAN_8
     };
  Max7219 max7219(PA_7, PA_6, PA_5, PA_4);//(SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_SS)
  max7219.set_num_devices(2);//2 devices
  max7219.init_display(cfg);//init all devices!
  max7219.enable_display();//enable all devices
  max7219.set_display_test();
  wait_ms(500);
  max7219.clear_display_test();
  max7219.display_all_off();
  wait_ms(1000);
  
  // common
  pc2.printf("Init common\r\n");
  SerialMidiHandler midiSerial(pcMidi);
  MidiHandler midiDummy;
  LogMidiHandler midiLog1(pc2, 1);
  MultiMidiHandler midiMult1(midiSerial, midiLog1, midiDummy, midiDummy);
  LogMidiHandler midiLog2(pc2, 2);
  MultiMidiHandler midiMult2(midiSerial, midiLog2, midiDummy, midiDummy);
  LogMidiHandler midiLog3(pc2, 3);
  MultiMidiHandler midiMult3(midiSerial, midiLog3, midiDummy, midiDummy);
  LogMidiHandler midiLog4(pc2, 4);
  MultiMidiHandler midiMult4(midiSerial, midiLog4, midiDummy, midiDummy);
  LogMidiHandler midiLog5(pc2, 5);
  MultiMidiHandler midiMult5(midiSerial, midiLog5, midiDummy, midiDummy);
  LogMidiHandler midiLog6(pc2, 6);
  MultiMidiHandler midiMult6(midiSerial, midiLog6, midiDummy, midiDummy);
  LogMidiHandler midiLog7(pc2, 7);
  MultiMidiHandler midiMult7(midiSerial, midiLog7, midiDummy, midiDummy);
  LogMidiHandler midiLog8(pc2, 8);
  MultiMidiHandler midiMult8(midiSerial, midiLog8, midiDummy, midiDummy);

  GateIn muteBtn(PB_12);
  GateIn setBtn(PB_13);
  GateIn clearBtn(PB_14);
  ToggleInOut learnModeBtn(PB_15, PC_14);//with indicator led
  GateIn clockIn(PA_8);//external clock input
  DigitalOut clockLed(PA_1);
  // debug serial
  // 8x track button 
  ToggleInOut playStepModeBtn(PB_8, PC_15);// play/Step mode toggle btn, with indicator led
  GateIn resetAdvanceBtn(PB_9);// reset/advance btn
  AnalogIn learnValuePot(PA_0);
  CommonState commonState;

  // multiple tracks
  pc2.printf("Init tracks\r\n");
  const uint8_t MidiNote = 0x23;
  const uint8_t MidiChannel = 0x00;
  TrackController track1(PA_11, midiSerial, MidiNote, MidiChannel);
  track1.SetPattern(0x1111);
  TrackController track2(PA_12, midiSerial, MidiNote+1, MidiChannel);
  TrackController track3(PA_15, midiSerial, MidiNote+2, MidiChannel);
  track3.SetPattern(0xFFFF);
  TrackController track4(PB_3, midiSerial, MidiNote+8, MidiChannel);
  TrackController track5(PB_4, midiSerial, MidiNote+10, MidiChannel);
  TrackController track6(PB_5, midiSerial, MidiNote+12, MidiChannel);
  TrackController track7(PB_6, midiSerial, MidiNote+14, MidiChannel);
  TrackController track8(PB_7, midiSerial, MidiNote+16, MidiChannel);
  track8.SetPattern(0xAAAA);
  const int NumTracks = 8;
  TrackController* tracks[] = {&track1, &track2, &track3, &track4, &track5, &track6, &track7, &track8};

  // 
  Timer timer;
  int counter = 0;
  const int fakeClockPeriod = 220;//1000;
  GateState fakeClock;

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
          muteBtn.Read();
          setBtn.Read();
          clearBtn.Read();
          learnModeBtn.Read();
          clockIn.Read();
          playStepModeBtn.Read();
          //fake clock
          fakeClock.Tick(repeat<fakeClockPeriod/2?1:0);

          commonState.mutePressed = muteBtn.Get();
          commonState.setPressed = setBtn.Get();
          commonState.clearPressed = clearBtn.Get();
          commonState.learnMode = learnModeBtn.Get();
          commonState.clockIsRising = clockIn.IsRising();
          commonState.clockIsFalling = clockIn.IsFalling();
          commonState.learnValue = learnValuePot.read();
          commonState.playMode = true;//playStepModeBtn.Get();

          // fake clock
          commonState.clockIsRising = fakeClock.IsRising();
          commonState.clockIsFalling = fakeClock.IsFalling();
          commonState.clockOn = fakeClock.Get();
          // TODO if play mode, use (fake) clock
          // TODO if step mode, use reset/advance btn
          
          track1.Tick(commonState);
          track2.Tick(commonState);
          track3.Tick(commonState);
          track4.Tick(commonState);
          track5.Tick(commonState);
          track6.Tick(commonState);
          track7.Tick(commonState);
          track8.Tick(commonState);
          // update display takes some time =>
          //update display? always? only upon clock rising/falling? also upon track button pressed?
          // alternating?
          {
            int trackIdx = repeat%NumTracks;
            //display track!
            uint32_t displayPattern = fakeClock.Get() ? tracks[trackIdx]->GetDisplayPattern() : tracks[trackIdx]->GetPattern();
            uint8_t digit = 8-trackIdx;//invert row [1,8]
            max7219.write_digit(1, digit, displayPattern&0xFF);
            max7219.write_digit(2, digit, (displayPattern>>8)&0xFF);
          }

          // fake clock
          clockLed = fakeClock.Get();
          ledOut = 1-fakeClock.Get();//inverted
          //ledOut = clockIn.Get()? 0:1;//inverted
          // 
          wait_ms(1);
          //wait_ms(1);
        }
      }
      
      timer.stop();
      pc2.printf("mute %d set %d clear %d learn %d\r\n", 
                  commonState.mutePressed?1:0, 
                  commonState.setPressed?1:0, 
                  commonState.clearPressed?1:0, 
                  commonState.learnMode?1:0);
      pc2.printf("%d : %d ms\r\n", counter++, timer.read_ms());
      
  }
}