#include <mbed.h>
#include "MidiParser.h"
#include "LogMidiHandler.h"
#include "MCP4x22.h"
#include "SerialBuffer.h"
#include "PwmVoltageOut.h"
#include "CVMidiHandler.h"
#include "MultiMidiHandler.h"
#include "ModeMidiHandler.h"
#include "SerialMidiHandler.h"
#include "PeriodicOut.h"
#include "MonophonicMidiHandler.h"
#include "GateMidiHandler.h"
#include "NPositionIn.h"
#include "ChannelFilterMidiHandler.h"

// globals because callback OnCharRecieved
SerialBuffer<100> serialBuffer;
Serial pcMidi(PA_2, PA_3); // tx, rx

void OnCharRecieved()
{
  serialBuffer.Write( pcMidi.getc());//getc is blocking!!!
}

int main() 
{
  DigitalOut ledOut(PC_13);//builtin led as gate indicator
//  DigitalIn clockIn(PA_12);//external clock input
  Serial pc2(PA_9, PA_10); // tx, rx

  //clock output
  PeriodicOutState clockState;
  DigitalOut clockOut(PB_13);
  DigitalOut clockLed(PA_11);

  // mode inputs (uses analog in)
  const int numModes = 3;
  NPositionIn modeIn1(A6, numModes);
  NPositionIn modeIn2(A5, numModes);
  NPositionIn modeIn3(A4, numModes);

  // clock speed in
  AnalogIn clockSpeedIn(A7);

  // apply settings
  clockState.SetPeriod(300);//approx 3.3 beat / second = 198 bpm
  clockState.SetDuration(0.5f);
  pc2.baud(115200);
  pcMidi.baud(31250);//midi baudrate

  // short pause before running
  wait_ms(2000);

  // start running
  pc2.printf("\r\n-\r\n-\r\nMidi multi CV...\r\n-\r\n-\r\n");
  pc2.printf("version 0.9\r\n");
  wait_ms(1000);

  // put your setup code here, to run once:
  MidiParser midiParser;
  Timer timer;
  int counter = 0;
  MidiHandler midiDummy;
  LogMidiHandler logHandlerCommon(pc2, 0);
  SerialMidiHandler serialMidiCommon(pcMidi);

  // channel 1: (poly) midi serial
  pc2.printf("create channel 1\r\n");
  LogMidiHandler logHandler1(pc2, 1);
  DigitalOut gateLed1(PA_12);
  GateMidiHandler midiGate1(gateLed1);
  MultiMidiHandler midiMulti1(logHandler1, midiDummy, midiGate1, serialMidiCommon);
  ModeMidiHandler modeHandler1(0, midiMulti1);
  modeHandler1.SetMode(ModeMidiHandler::LivePoly);

  // channel 2:  (mono) CV  out + midi serial
  wait_ms(500);
  pc2.printf("create channel 2\r\n");
  LogMidiHandler logHandler2(pc2, 2);
  // outputs for CV out channel 2
  DigitalOut gateOut2(PA_8);//gate output 
  PwmVoltageOut voltageOutPitch2(PB_0);//PWM voltage output for 1V/oct
  PwmVoltageOut voltageOutVelocity2(PB_1);//PWM voltage output for velocity
  CVMidiHandler midiCV2(voltageOutPitch2, voltageOutVelocity2, gateOut2);
  MonophonicMidiHandler midiMono2(midiCV2);
  // gate led
  DigitalOut gateLed2(PA_15);
  GateMidiHandler midiGate2(gateLed2);
  // midi serial
  // multi
  MultiMidiHandler midiMulti2(logHandler2, midiMono2, midiGate2, serialMidiCommon);
  ModeMidiHandler modeHandler2(1, midiMulti2);
  modeHandler2.SetMode(ModeMidiHandler::LivePoly);

  // channel 3: stepper to midi serial and (mono) CV out 
  wait_ms(500);
  pc2.printf("create channel 3\r\n");
  LogMidiHandler logHandler3(pc2, 3);
  DigitalOut gateOut3(PB_15);//gate output 
  PwmVoltageOut voltageOutPitch3(PB_10);//PWM voltage output for 1V/oct
  PwmVoltageOut voltageOutVelocity3(PB_11);//PWM voltage output for velocity
  CVMidiHandler midiCV3(voltageOutPitch3, voltageOutVelocity3, gateOut3);
  MonophonicMidiHandler midiMono3(midiCV3);
  DigitalOut gateLed3(PB_3);
  GateMidiHandler midiGate3(gateLed3);
  MultiMidiHandler midiMulti3(logHandler3, midiMono3, midiGate3, serialMidiCommon);
  ModeMidiHandler modeHandler3(2, midiMulti3);
  modeHandler3.SetMode(ModeMidiHandler::StepperRecord);

  // channel [4-16]: midi thru to midi serial 
  wait_ms(500);
  pc2.printf("create channel [4-16]\r\n");
  LogMidiHandler logHandler4(pc2, 4);
  MultiMidiHandler midiMult4;
  midiMult4.AddHandler(&logHandler4);
  midiMult4.AddHandler(&serialMidiCommon);
  ChannelFilterMidiHandler midiHandler4(midiMult4);
  midiHandler4.AllowChannel(0,false);
  midiHandler4.AllowChannel(1,false);
  midiHandler4.AllowChannel(2,false);
  
  wait_ms(500);
  pc2.printf("create multi\r\n");
  MultiMidiHandler midiMulti(modeHandler1, modeHandler2, modeHandler3, midiHandler4);
  midiMulti.AddHandler(&logHandlerCommon);

  wait_ms(500);
  pc2.printf("start midi listening\r\n");
  pcMidi.attach(&OnCharRecieved, Serial::RxIrq);

  wait_ms(500);
  pc2.printf("start processing\r\n");
  while(1) {
      // put your main code here, to run repeatedly:
      timer.reset();
      timer.start();

      //int clockPeriod = 300;

      int numRead = 0;
      for(int repeat = 0; repeat<1000; ++repeat)
      {
        // read midi in -> handle midi
        uint8_t byte = 0x00;
        while(serialBuffer.Read(byte))
        {
          ++numRead;
          midiParser.Parse(byte, midiMulti);
        }

        // do some timing sensitive stuff
        clockState.Tick(false);
        modeHandler1.Tick(clockState.Get());
        modeHandler2.Tick(clockState.Get());
        modeHandler3.Tick(clockState.Get());
        // TODO read reset from clock input  clockIn.read()

        // clock led and gate out
        ledOut = clockState.Get() ? 0:1;//need to invert led
        clockOut = clockState.Get() ? 1:0;
        clockLed = clockState.Get() ? 1:0;

        int analogRead = repeat%4;
        if(analogRead==0)
        {
          // read pots, one at a time : mode in, clock speed
          // clock period in  [125, 125+2048]
          int currClockPeriod = 60 + (clockSpeedIn.read_u16()>>5);
          clockState.SetPeriod(currClockPeriod);
        } 
        else if(analogRead==1)
        {
          modeIn1.Read();
          ModeMidiHandler::Mode mode = static_cast<ModeMidiHandler::Mode>(modeIn1.Get());
          modeHandler1.SetMode(mode);
        }
        else if(analogRead==2)
        {
          modeIn2.Read();
          ModeMidiHandler::Mode mode = static_cast<ModeMidiHandler::Mode>(modeIn2.Get());
          modeHandler2.SetMode(mode);
        }
        else if(analogRead==3)
        {
          modeIn3.Read();
          ModeMidiHandler::Mode mode = static_cast<ModeMidiHandler::Mode>(modeIn3.Get());
          modeHandler3.SetMode(mode);
        }
        // 
        wait_ms(1);
      }

      timer.stop();
      pc2.printf("%d : %d ms read %d\r\n", counter++, timer.read_ms(), numRead);

      //pc2.printf("%d %d %d\r\n", modeIn1.Get(), modeIn2.Get(), modeIn3.Get());
  }
}
