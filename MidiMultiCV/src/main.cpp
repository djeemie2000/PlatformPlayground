#include <mbed.h>
#include "MidiParser.h"
#include "LogMidiHandler.h"
#include "MCP4x22.h"
#include "SerialBuffer.h"
#include "VoltageOut.h"

SerialBuffer<100> serialBuffer;
Serial pcMidi(PA_2, PA_3); // tx, rx


void OnCharRecieved()
{
  serialBuffer.Write( pcMidi.getc());//getc is blocking!!!
//  uint8_t byte = pPcMidi->getc();
  //pPc2->printf("0x%2x ", byte);
  //pMidiParser->Parse(byte, *pMidiHandler);
}

int main() {
  DigitalOut ledOut(PC_13);//builtin led as gate indicator
  DigitalIn clockIn(PA_12);//clock input
  Serial pc2(PA_9, PA_10); // tx, rx
  DigitalOut gateOut(PA_15);//gate output
  VoltageOut voltageOut(PB_1);//PWM voltage output for 1V/oct

    pc2.baud(115200);
    wait_ms(4000);
    pc2.printf("\r\n-\r\n-\r\nMidi multi CV...\r\n-\r\n-\r\n");
    //wait_ms(1000);

    pcMidi.baud(31250);//midi baudrate

    // put your setup code here, to run once:
    MidiParser midiParser;
    Timer timer;
    int counter = 0;
    LogMidiHandler midiHandler(pc2);

    pcMidi.attach(&OnCharRecieved, Serial::RxIrq);

    while(1) {
        // put your main code here, to run repeatedly:
        timer.reset();
        timer.start();

        int numRead = 0;
        for(int repeat = 0; repeat<500; ++repeat)
        {
          uint8_t byte = 0x00;
          //midi thru (for now)
          while(serialBuffer.Read(byte))
          {
            ++numRead;
            midiParser.Parse(byte, midiHandler);
          }
          // do some timing sensitive stuff
          wait_ms(1);
        }
        // toggle led
        ledOut = !ledOut;

        if(counter%2)
        {
          // note on
          pcMidi.putc(0x9F);
          pcMidi.putc(0x45);
          pcMidi.putc(0x40);

          gateOut = 1;
          // 7/12, 1+ 7/12, 2, 2+7/12, 3
          int notes[] = {0,2,4,5,7,9,11,12};
          int tmp = (counter/2)%8;
          int cv = 12+notes[tmp];

          voltageOut.write(cv/12.0f);

          pc2.printf("Note on %d\r\n", tmp);
        }
        else
        {
          // note off
          pcMidi.putc(0x8F);
          pcMidi.putc(0x45);
          pcMidi.putc(0x40);

          gateOut = 0;
          //voltageOut.write(1.0f);//0,1,2,3 volts

          pc2.printf("Note off\r\n");
        }


        timer.stop();
        pc2.printf("\r\ntime=%f seconds, count=%d numRead=%d \r\n", timer.read(), counter++, numRead);
    }
}
