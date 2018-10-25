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

  // mode inputs (uses analog in)
  const int numModes = 3;
  NPositionIn modeIn1(A4, numModes);
  NPositionIn modeIn2(A5, numModes);
  NPositionIn modeIn3(A6, numModes);

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
  pc2.printf("version 0.3\r\n");
  wait_ms(1000);

  // put your setup code here, to run once:
  MidiParser midiParser;
  Timer timer;
  int counter = 0;
  MidiHandler midiDummy;
  LogMidiHandler logHandlerCommon(pc2, 0);

  // channel 1: live (poly) to midi serial
  pc2.printf("create channel 1\r\n");
  LogMidiHandler logHandler1(pc2, 1);
  SerialMidiHandler midiHandler1(pcMidi);
  DigitalOut gateLed1(PA_11);
  GateMidiHandler midiGate1(gateLed1);
  MultiMidiHandler midiMulti1(logHandler1, midiHandler1, midiGate1, midiDummy);
  ModeMidiHandler modeHandler1(0, midiMulti1);
  modeHandler1.SetMode(ModeMidiHandler::LivePoly);

  // channel 2: live to (mono)  CV + midi serial
  wait_ms(500);
  pc2.printf("create channel 2\r\n");
  LogMidiHandler logHandler2(pc2, 2);
  // outputs for CV out channel 2
  DigitalOut gateOut2(PB_4);//gate output 
  PwmVoltageOut voltageOutPitch2(PB_0);//PWM voltage output for 1V/oct
  PwmVoltageOut voltageOutVelocity2(PB_1);//PWM voltage output for velocity
  CVMidiHandler midiHandler2(voltageOutPitch2, voltageOutVelocity2, gateOut2);
  MonophonicMidiHandler midiMono2(midiHandler2);
  // gate led
  DigitalOut gateLed2(PA_12);
  GateMidiHandler midiGate2(gateLed2);
  // midi serial
  SerialMidiHandler midiHandler2b(pcMidi);
  // multi
  MultiMidiHandler midiMulti2(logHandler2, midiMono2, midiGate2, midiHandler2b);
  ModeMidiHandler modeHandler2(1, midiMulti2);
  modeHandler2.SetMode(ModeMidiHandler::LivePoly);

  // channel 3: stepper to midi serial and (mono) CV out 
  wait_ms(500);
  pc2.printf("create channel 3\r\n");
  LogMidiHandler logHandler3(pc2, 3);
  SerialMidiHandler midiHandler3(pcMidi);
  DigitalOut gateOut3(PB_5);//gate output 
  PwmVoltageOut voltageOutPitch3(PB_10);//PWM voltage output for 1V/oct
  PwmVoltageOut voltageOutVelocity3(PB_11);//PWM voltage output for velocity
  CVMidiHandler midiHandler3b(voltageOutPitch3, voltageOutVelocity3, gateOut3);
  MonophonicMidiHandler midiMono3(midiHandler3);
  DigitalOut gateLed3(PA_15);
  GateMidiHandler midiGate3(gateLed3);
  MultiMidiHandler midiMulti3(logHandler3, midiHandler3, midiGate3, midiHandler3b);
  ModeMidiHandler modeHandler3(2, midiMulti3);
  modeHandler3.SetMode(ModeMidiHandler::StepperRecord);

  wait_ms(500);
  pc2.printf("create multi\r\n");
  MultiMidiHandler midiMulti(logHandlerCommon, modeHandler1, modeHandler2, modeHandler3);

  wait_ms(500);
  pc2.printf("start midi listening\r\n");
  pcMidi.attach(&OnCharRecieved, Serial::RxIrq);

  wait_ms(500);
  pc2.printf("start processing\r\n");
  while(1) {
      // put your main code here, to run repeatedly:
      timer.reset();
      timer.start();

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
        ledOut = clockState.Get() ? 0:1;//need to invert led??
        clockOut = clockState.Get() ? 1:0;

        // read pots, one at a time : mode in, clock speed

        // 
        wait_ms(1);
      }

      timer.stop();
      pc2.printf("\r\ntime=%f seconds, count=%d numRead=%d \r\n", timer.read(), counter++, numRead);
  }
}
