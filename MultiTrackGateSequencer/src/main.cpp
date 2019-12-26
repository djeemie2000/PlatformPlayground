#include <mbed.h>
#include "GateIn.h"
#include "TrackPlayer.h"
#include "CommonState.h"
#include "TrackController.h"
#include "Mpr121InBank.h"
#include "Max7219Matrix.h"
#include "BitWise.h"
#include "ScanI2C.h"
#include "ClockInQuantizer.h"

//#include "MBedUnit.h"

int main() {

  // put your setup code here, to run once:
  DigitalOut ledOut(PC_13);//builtin led as gate indicator
  Serial debugSerial(PA_9, PA_10); // tx, rx
  debugSerial.baud(115200);
//  Serial pcMidi(PA_2, PA_3); // tx, rx
//  pcMidi.baud(31250);//midi baudrate

  // short pause before running
  wait_us(2000000l);

  //testMain(debugSerial);//TODO debug

  // start running
  debugSerial.printf("\r\n-\r\n-\r\nMulti track gate sequencer...\r\n-\r\n-\r\n");
  debugSerial.printf("version 0.5\r\n");
  wait_us(1000000l);

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
  //SerialMidiHandler midiSerial(pcMidi);

  DigitalOut clockLed(PB_14);
  GateIn clockIn(PB_13);//external clock input
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
  const uint32_t patterns[] = {0x11111111, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x55555555, 0xAAAAAAAA, 0x00000000, 0xAA88AA88};
  const PinName outPins[] = {PA_11, PA_12, PA_15, PB_3, PB_4, PB_5, PB_6, PB_7 };
  DigitalOut* midiOutputs[NumTracks];
  DigitalOutGateHandler* multis[NumTracks];//needed???

  TrackController* tracks[NumTracks];
  for(int idx = 0; idx<NumTracks; ++idx)
  {
    // yes, these are memory leaks (if we would ever destruct)
    midiOutputs[idx] = new DigitalOut(outPins[idx]);
    multis[idx] = new DigitalOutGateHandler(*midiOutputs[idx]);
    tracks[idx] = new TrackController(*multis[idx], idx, ledMatrix);
    tracks[idx]->SetPattern(patterns[idx], PatternLength);
  }

  // 
  Timer timer;
  int counter = 0;
  const int fakeClockPeriod = 220;
  ClockInState clockInState(fakeClockPeriod);

  wait_us(500000l);
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
          touchPad.Read();

          // use pad 8-11 for set/clear/mute(toggle)/reset
          //TODO duration HW support
          commonState.setPressed = touchPad.Get(8);
          commonState.clearPressed = touchPad.Get(9);
          commonState.mutePressed = touchPad.Get(10);
          commonState.resetStepPressed = touchPad.Get(11);
          
          clockInState.Tick(clockIn.IsRising(), true);
          commonState.clockCntr = clockInState.Cntr();
          commonState.clockPeriod = clockInState.Period();

          // clock
          commonState.clockIsRising = clockIn.IsRising();
          commonState.clockIsFalling = clockIn.IsFalling();
          commonState.clockOn = clockIn.Get();

          for(int idx = 0; idx<NumTracks; ++idx)
          {
            const int allTrackBtn = 0;// not yet HW support for 'all track' button 
            tracks[idx]->Tick(commonState, touchPad.Get(idx), allTrackBtn);
          }          

          // update display takes some time => update alternating rows
          ledMatrix.Write(repeat%NumTracks);

          // clock
          clockLed = clockIn.Get();
          ledOut = 1-clockIn.Get();//inverted

          // 
          wait_us(1000);
        }
      }
      
      timer.stop();
      debugSerial.printf("mute %d set %d clear %d ext %d", 
                  commonState.mutePressed?1:0, 
                  commonState.setPressed?1:0, 
                  commonState.clearPressed?1:0, 
                  clockInState.Period()
                  );
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
