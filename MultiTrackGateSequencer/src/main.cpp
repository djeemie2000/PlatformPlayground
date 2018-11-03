#include <mbed.h>
#include "GateIn.h"
#include "GateTrack.h"
#include "LogMidiHandler.h"
#include "CommonState.h"
#include "TrackController.h"
#include "max7219.h"

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
  wait_ms(1000);
  max7219.clear_display_test();
  max7219.display_all_off();
  wait_ms(1000);
  
  // common
  pc2.printf("Init common\r\n");
  LogMidiHandler midiLog1(pc2, 1);
  LogMidiHandler midiLog2(pc2, 2);
  LogMidiHandler midiLog3(pc2, 3);
  LogMidiHandler midiLog4(pc2, 4);
  LogMidiHandler midiLog5(pc2, 5);
  LogMidiHandler midiLog6(pc2, 6);
  LogMidiHandler midiLog7(pc2, 7);
  LogMidiHandler midiLog8(pc2, 8);

  GateIn muteBtn(PB_12);
  GateIn setBtn(PB_13);
  GateIn clearBtn(PB_14);
  ToggleInOut learnModeBtn(PB_15, PC_14);//with indicator led
  GateIn clockIn(PA_8);//external clock input
  // debug serial
  // 8x track button 
  ToggleInOut playStepModeBtn(PB_8, PC_15);// play/Step mode toggle btn, with indicator led
  GateIn resetAdvanceBtn(PB_9);// reset/advance btn
  AnalogIn learnValuePot(PA_0);
  CommonState commonState;

  // track
  pc2.printf("Init tracks\r\n");
  //TODO multiple tracks
  const uint8_t MidiNote = 0x40;
  const uint8_t MidiChannel = 0x04;
  TrackController track1(PA_11, midiLog1, MidiNote, MidiChannel);
  track1.SetPattern(0x5555);// 10101010 10101010
  TrackController track2(PA_12, midiLog2, MidiNote+1, MidiChannel);
  TrackController track3(PA_15, midiLog3, MidiNote+2, MidiChannel);
  TrackController track4(PB_3, midiLog4, MidiNote+3, MidiChannel);
  TrackController track5(PB_4, midiLog5, MidiNote+4, MidiChannel);
  TrackController track6(PB_5, midiLog6, MidiNote+5, MidiChannel);
  TrackController track7(PB_6, midiLog7, MidiNote+6, MidiChannel);
  TrackController track8(PB_7, midiLog8, MidiNote+7, MidiChannel);
  const int NumTracks = 8;
  TrackController* tracks[] = {&track1, &track2, &track3, &track4, &track5, &track6, &track7, &track8};
//  GateIn trackBtn(PA_11);
//  GateTrackPlayer track(midiLog);
//  track.Learn(MidiNote, MidiChannel);

  // 
  Timer timer;
  int counter = 0;

  wait_ms(500);
  pc2.printf("start processing\r\n");
  while(1) {
    // put your main code here, to run repeatedly:
      timer.reset();
      timer.start();

      for(int repeat = 0; repeat<500; ++repeat)
      {
        muteBtn.Read();
        setBtn.Read();
        clearBtn.Read();
        learnModeBtn.Read();
        clockIn.Read();
        
        //
        commonState.mutePressed = muteBtn.Get();
        commonState.setPressed = setBtn.Get();
        commonState.clearPressed = clearBtn.Get();
        commonState.learnMode = learnModeBtn.Get();
        commonState.clockIsRising = clockIn.IsRising();
        commonState.clockIsFalling = clockIn.IsFalling();
        commonState.learnValue = learnValuePot.read();

        // fake clock
        commonState.clockIsRising = (repeat==0);
        commonState.clockIsFalling = (repeat==5000);
        
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
        //TODO use tracks[]
        int trackIdx = repeat%NumTracks;
        {
          //display track!
          uint32_t displayPattern = tracks[trackIdx]->GetDisplayPattern();
          uint8_t digit = 1+trackIdx;//Max7219::MAX7219_DIGIT_0;
          max7219.write_digit(1, digit, displayPattern&0xFF);
          max7219.write_digit(2, digit, (displayPattern>>8)&0xFF);
        }

        // fake clock
        ledOut = repeat<500?0:1;
        //ledOut = clockIn.Get()? 0:1;//inverted
        // 
        wait_ms(1);
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