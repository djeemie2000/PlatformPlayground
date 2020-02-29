#include <arduino.h>
#include "GateIn.h"
#include "TrackPlayer.h"
#include "CommonState.h"
#include "TrackController.h"
#include "Mpr121Touchpad.h"
#include "TTP8229TouchPad.h"
#include "Max7219Matrix.h"
#include "BitWise.h"
#include "ScanI2C.h"
#include "ClockInQuantizer.h"
#include "MemController.h"
#include "Eeprom24LC256.h"
#include "DigitalOut.h"
#include "Timer.h"
#include "Wait.h"
#include "SerialOut.h"

//#include "MBedUnit.h"
#include "TestDigitalOutMatrix.h"
#include "TestTouchPad.h"
#include "TestMemoryBank.h"
#include "TestGSData.h"


void setup()
{

}

void loop() 
{
// pin usage:
// track digital outs : pin [2,9]
// SPI 10,11,12,13 => cannot use LED pin 13
// I2C A4,A5
// clock digital in: A0 (pullup!)
// debug on/off digital in : A1 (pullup!)
// clockLed out : A3
// 
// led matrix Max8501 to SPI
// TTP82999 touchpad to I2C adress 0x57
// EEPROM to I2C addres ???

  // put your setup code here, to run once:
  //DigitalOut ledOut(LED_BUILTIN);//builtin led as gate indicator -> no, SPI uses this pin!!!
  SerialOut debugSerial;
  unsigned long baudrate = 115200ul;
  debugSerial.begin(baudrate);

  // short pause before running
  wait_ms(500);

  // start running
  debugSerial.println("GateSeq8");
  debugSerial.println("Multi track gate sequencer...");
  debugSerial.println("version 0.2");
  delay(500);

  //return;//Debug  
  int tmp = 12345;
  int tmp3 = 0x5A;
  unsigned long tmp2 = 0x1234ABCD;
  debugSerial.printf("test printf...\r\n int %d hex 0x%x hex_u32 %x --eol-\r\n", tmp, tmp3, tmp2);

  ScanI2C(debugSerial);
  delay(2000);
  
  // set, mute, clear, reset btn 
  // select bank,  select pattern, save, tbc 
  // 8x track button 
  debugSerial.println("Init touchpad...");
  TTP8229TouchPad touchPad;
  touchPad.Begin(TTP8229TouchPad::I2CMode);
  //TestTouchPad(touchPad, debugSerial);
  //return; //Debug

  //
  debugSerial.println("Init led matrix...");
  Max7219Matrix ledMatrix(4, PIN_SPI_SS);// chipselect pin 10 (SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_SS));//4 devices
  ledMatrix.Configure();
  TestDigitalOutMatrix(ledMatrix, debugSerial,30);
  //return;

  // common
  debugSerial.print("Init common\r\n");
  DigitalOut clockLed(PIN_A3);
  GateIn clockIn(PIN_A0, true);//external clock input, pullup
  GateIn debugActiveIn(PIN_A1,true);//pullup => default on (????)
  CommonState commonState;
  
  // memory
  debugSerial.println("Init memory bank");
  Eeprom24LC256 memBank(Eeprom24LC256::AllLow);
  
  // for(int bank = 18; bank<20; ++bank)
  // {
  //   TestMemoryBank(memBank, debugSerial, bank);
  //   delay(500);
  // }
  for(int bank = 0; bank<5; ++bank)
  {
    PrintMemoryBank(memBank, debugSerial, bank, 32, 0);
    delay(2000);
  }
  // return;

  debugSerial.println("test GS data memory...");
  GSCommon testGSCommon;
  LoadGSCommon(memBank, 0,0, testGSCommon);
  PrintGSCommon(testGSCommon, debugSerial);
  delay(2000);
  for(int idx = 0; idx<8; ++idx)
  {
    GSTrack testGSTrack;
    LoadGSTrack(memBank, 1+idx, 0, testGSTrack);
    PrintGSTrack(testGSTrack, debugSerial);
//    testGSTrack.m_Pattern = 0xAA8C47F1;
    testGSTrack.m_NumSteps = 32;
    SaveGSTrack(memBank, 1+idx, 0, testGSTrack);
//    Init(testGSTrack, 32);
    LoadGSTrack(memBank, 1+idx, 32, testGSTrack);
    PrintGSTrack(testGSTrack, debugSerial);
    testGSTrack.m_NumSteps = 32;
    SaveGSTrack(memBank, 1+idx, 32, testGSTrack);
    delay(2000);
  }
  //delay(8000);
  //return;

  debugSerial.println("Init memory controller");
  MemController memController(memBank);
  debugSerial.println("Init memory controller done");
  //load all patterns -> indicator on led matrix
  for(int selectedBank = 0; selectedBank<GSMem::NumBanks; ++selectedBank)
  {
      for(int selectedPattern = 0; selectedPattern<GSBank::NumPatterns; ++selectedPattern)
      {
//          debugSerial.printf("load pattern bank %d pattern %d\r\n", selectedBank, selectedPattern);
          ledMatrix.Set(selectedBank,selectedPattern);
          memController.LoadPattern(selectedBank, selectedPattern);
          ledMatrix.WriteAll();//WriteRow(selectedBank);
      }
  }
  debugSerial.println("load common");
  memController.LoadCommon();
  
  debugSerial.println("Init tracks");
  // multiple tracks
  const int PatternLength = 32;
  //default pattern, gate outputs & handlers
  //  const unsigned long patterns[] = {0x11111111, 0x10101010, 0x00000000, 0x55555555, 0xAAAAAAAA, 0x00000000, 0x10001000 ,0xFFFFFFFF};

  const uint8_t outPins[] = {2, 3, 4, 5, 6, 7, 8, 9 };
  DigitalOutGateHandler* gateOutHandlers[GSPattern::NumTracks];
  TrackController* trackControllers[GSPattern::NumTracks];
 
  for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
  {
    // yes, these are memory leaks (if we would ever destruct)
    gateOutHandlers[idx] = new DigitalOutGateHandler(outPins[idx]);
    trackControllers[idx] = new TrackController(*gateOutHandlers[idx], idx, &(memController.GetCurrentPattern()->m_Track[idx]));
  }

  // 
  Timer timer;
  int counter = 0;
  const int fakeClockPeriod = 110;
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
          touchPad.Read();

          // use pad 8-11 for set/clear/mute(toggle)/reset
          //TODO duration HW support
          commonState.setPressed = touchPad.Get(8);
          commonState.clearPressed = touchPad.Get(9);
          commonState.mutePressed = touchPad.Get(10);
          commonState.resetStepPressed = touchPad.Get(11);

          commonState.selectPatternPressed = touchPad.Get(12);
          commonState.selectBankPressed = touchPad.Get(13);
          commonState.savePatternsPressed = touchPad.Get(14);

//          clockInState.Tick(repeat==0, true);
          clockInState.Tick(clockIn.IsRising(), true);
          commonState.clockCntr = clockInState.Cntr();
          commonState.clockPeriod = clockInState.Period();

          for(int idx = 0; idx<GSPattern::NumTracks; ++idx)
          {
            trackControllers[idx]->Tick(commonState, touchPad.Get(idx), memController);
          }          

          // update display takes some time => update alternating rows
          trackControllers[repeat%GSPattern::NumTracks]->Update(ledMatrix);
          ledMatrix.WriteRow(repeat%GSPattern::NumTracks);

          // clock
          clockLed = clockIn.Get();
          //ledOut = 1-clockIn.Get();//inverted

          // 
          wait_ms(1);
        }
      }
      
      timer.stop();

      debugActiveIn.Read();
      if(debugActiveIn.Get())
      {
        debugSerial.printf("mute %d set %d clear %d bank %d ptrn %d save %d clk %d", 
                    commonState.mutePressed?1:0, 
                    commonState.setPressed?1:0, 
                    commonState.clearPressed?1:0, 
                    commonState.selectBankPressed?1:0,
                    commonState.selectPatternPressed?1:0,
                    commonState.savePatternsPressed?1:0,
                    clockInState.Period()
                    );
        debugSerial.printf("touchpad %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", 
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
                    touchPad.Get(11),
                    touchPad.Get(12),
                    touchPad.Get(13),
                    touchPad.Get(14));
        debugSerial.printf("%d : %d ms\r\n", counter++, timer.read_ms());
      }
  }
}
