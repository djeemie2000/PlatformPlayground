#include <mbed.h>
#include "GateIn.h"
#include "GateTrack.h"
#include "LogMidiHandler.h"
#include "CommonState.h"
#include "TrackController.h"
#include "SerialMidiHandler.h"
#include "MultiMidiHandler.h"
#include "PeriodicOut.h"
#include "Mpr121InBank.h"
#include "Max7219Matrix.h"
#include "BitWise.h"

void ScanI2C(I2C& i2c, Serial& serial)
{
  serial.printf("I2C scanning...\r\n");
  for(int address = 1; address < 127; address++ ) 
  {
    i2c.start();
    int error = i2c.write(address << 1); //We shift it left because mbed takes in 8 bit addreses
    i2c.stop();
    if (error == 1)
    {
      serial.printf("I2C device found at address 0x%X\r\n", address); //Returns 7-bit addres
    }
  }
  serial.printf("I2C scan done\r\n");
}

int main() {

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
  Max7219Matrix ledMatrix(2, PA_7, PA_6, PA_5, PA_4);//(SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_SS));//2 devices
  ledMatrix.Configure();
  ledMatrix.Test();
  
  // common
  pc2.printf("Init common\r\n");
  SerialMidiHandler midiSerial(pcMidi);

  GateIn muteBtn(PB_12);
  GateIn setBtn(PB_13);
  GateIn clearBtn(PB_14);
  ToggleInOut learnModeBtn(PB_15, PC_14);//with indicator led
  GateIn clockIn(PA_8);//external clock input
  DigitalOut clockLed(PA_1);
  // debug serial
  //ToggleInOut playStepModeBtn(PB_8, PC_15);// play/Step mode toggle btn, with indicator led
  GateIn resetAdvanceBtn(PB_9);// reset/advance btn
  AnalogIn learnValuePot(PA_0);
  CommonState commonState;
  I2C i2c(PB_11, PB_10);
  ScanI2C(i2c, pc2);
  // set, mute, clear btn, 8x track button 
  Mpr121InBank touchPad(&i2c, PB_1);

  // multiple tracks
  pc2.printf("Init tracks\r\n");
  const uint8_t MidiNote = 0x23;
  const uint8_t MidiChannel = 0x03;//channel 4
  TrackController track1(midiSerial, MidiNote, MidiChannel, 0, ledMatrix);
  track1.SetPattern(0x1111);
  TrackController track2(midiSerial, MidiNote+1, MidiChannel, 1, ledMatrix);
  TrackController track3(midiSerial, MidiNote+2, MidiChannel, 2, ledMatrix);
  track3.SetPattern(0xFFFF);
  TrackController track4(midiSerial, MidiNote+8, MidiChannel, 3, ledMatrix);
  TrackController track5(midiSerial, MidiNote+10, MidiChannel, 4, ledMatrix);
  TrackController track6(midiSerial, MidiNote+12, MidiChannel, 5, ledMatrix);
  TrackController track7(midiSerial, MidiNote+14, MidiChannel, 6, ledMatrix);
  TrackController track8(midiSerial, MidiNote+16, MidiChannel, 7, ledMatrix);
  track8.SetPattern(0xAA88);
  const int NumTracks = 8;
  TrackController* tracks[] = {&track1, &track2, &track3, &track4, &track5, &track6, &track7, &track8};

  // 
  Timer timer;
  int counter = 0;
  const int fakeClockPeriod = 220;
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
          //playStepModeBtn.Read();
          resetAdvanceBtn.Read();
          touchPad.Read();
          //fake clock
          fakeClock.Tick(repeat<fakeClockPeriod/2?1:0);

          commonState.mutePressed = touchPad.Get(1);//muteBtn.Get();
          commonState.setPressed = touchPad.Get(0);//setBtn.Get();
          commonState.clearPressed = touchPad.Get(2);//clearBtn.Get();
          commonState.learnMode = learnModeBtn.Get();
          commonState.clockIsRising = clockIn.IsRising();
          commonState.clockIsFalling = clockIn.IsFalling();
          commonState.learnValue = learnValuePot.read();
          commonState.playMode = true;//playStepModeBtn.Get();

          // fake clock
          // if play mode, use (fake) clock
          // if step mode, use reset/advance btn
          if(commonState.playMode)
          {
            commonState.clockIsRising = fakeClock.IsRising();
            commonState.clockIsFalling = fakeClock.IsFalling();
            commonState.clockOn = fakeClock.Get();
          }
          else
          {
            commonState.clockIsRising = resetAdvanceBtn.IsRising();
            commonState.clockIsFalling = resetAdvanceBtn.IsFalling();
            commonState.clockOn = resetAdvanceBtn.Get();
          }

          for(int idx = 0; idx<NumTracks; ++idx)
          {
            int allTrackBtn = 0;
            tracks[idx]->Tick(commonState, touchPad.Get(4+idx), allTrackBtn);
          }          

          // update display takes some time => alternating rows
          int trackIdx = repeat%NumTracks;
          ledMatrix.Write(trackIdx);

          // fake clock
          clockLed = fakeClock.Get();
          ledOut = 1-fakeClock.Get();//inverted
          //ledOut = clockIn.Get()? 0:1;//inverted
          // 
          wait_ms(1);
        }
      }
      
      timer.stop();
      pc2.printf("mute %d set %d clear %d learn %d play %d ", 
                  commonState.mutePressed?1:0, 
                  commonState.setPressed?1:0, 
                  commonState.clearPressed?1:0, 
                  commonState.learnMode?1:0,
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