#include <mbed.h>
#include "MidiParser.h"
#include "LogMidiHandler.h"
#include "MCP4x22.h"
#include "SerialBuffer.h"
#include "VoltageOut.h"
#include "CVMidiHandler.h"
#include "MultiMidiHandler.h"
#include "ModeMidiHandler.h"
#include "SerialMidiHandler.h"
#include "PeriodicOut.h"

// globals because callback OnCharRecieved
SerialBuffer<100> serialBuffer;
Serial pcMidi(PA_2, PA_3); // tx, rx

void OnCharRecieved()
{
  serialBuffer.Write( pcMidi.getc());//getc is blocking!!!
}

int main() {
  DigitalOut ledOut(PC_13);//builtin led as gate indicator
  DigitalIn clockIn(PA_12);//external clock input
  DigitalIn modeToggleIn(PA_11);//mode toggle input
  Serial pc2(PA_9, PA_10); // tx, rx
  DigitalOut gateOut(PA_15);//gate output
  PwmVoltageOut voltageOut(PB_1);//PWM voltage output for 1V/oct
  PwmVoltageOut voltageOut2(PB_0);//PWM voltage output for velocity
  PeriodicOutState clockState;
  DigitalOut clockOut(PB_3);//clock output


  clockState.SetPeriod(500);//approx 2 beat / second = 120 bpm
  clockState.SetDuration(0.5f);
  pc2.baud(115200);
  pcMidi.baud(31250);//midi baudrate
  wait_ms(4000);

  pc2.printf("\r\n-\r\n-\r\nMidi multi CV...\r\n-\r\n-\r\n");
  pc2.printf("version 0.1\r\n");
  //wait_ms(1000);

  // put your setup code here, to run once:
  MidiParser midiParser;
  Timer timer;
  int counter = 0;
  MidiHandler dummy;
  LogMidiHandler logHandlerCommon(pc2, 0);

  // channel 1: live (poly) to midi serial
  LogMidiHandler logHandler1(pc2, 1);
  SerialMidiHandler midiHandler1(pcMidi);
  MultiMidiHandler midiMulti1(logHandler1, midiHandler1, dummy, dummy);
  ModeMidiHandler modeHandler1(0, midiMulti1);
  modeHandler1.SetMode(ModeMidiHandler::LivePoly);

  // channel 2: live to CV
  CVMidiHandler midiHandler2(voltageOut, voltageOut2, gateOut);
  LogMidiHandler logHandler2(pc2, 2);
  MultiMidiHandler midiMulti2(logHandler2, midiHandler2, dummy, dummy);
  ModeMidiHandler modeHandler2(1, midiMulti2);
  modeHandler2.SetMode(ModeMidiHandler::LiveMono);//!!

  // channel 3: stepper to midi serial
  LogMidiHandler logHandler3(pc2, 3);
  SerialMidiHandler midiHandler3(pcMidi);
  MultiMidiHandler midiMulti3(logHandler3, midiHandler3, dummy, dummy);
  ModeMidiHandler modeHandler3(0, midiMulti3);
  modeHandler3.SetMode(ModeMidiHandler::StepperRecord);

  MultiMidiHandler midiMulti(logHandlerCommon, modeHandler1, modeHandler2, modeHandler3);

  pcMidi.attach(&OnCharRecieved, Serial::RxIrq);

  while(1) {
      // put your main code here, to run repeatedly:
      timer.reset();
      timer.start();

      int numRead = 0;
      for(int repeat = 0; repeat<1000; ++repeat)
      {
        uint8_t byte = 0x00;
        //midi thru (for now)
        while(serialBuffer.Read(byte))
        {
          ++numRead;
          midiParser.Parse(byte, midiMulti);
        }
        // do some timing sensitive stuff
        // TODO read reset from clock input  clockIn.read()
        clockState.Tick(false);
        modeHandler1.Tick(clockState.Get());
        modeHandler2.Tick(clockState.Get());

        ledOut = clockState.Get() ? 0:1;//need to invert led??
        clockOut = clockState.Get() ? 1:0;

        wait_ms(1);
      }

      timer.stop();
      pc2.printf("\r\ntime=%f seconds, count=%d numRead=%d \r\n", timer.read(), counter++, numRead);
  }
}
