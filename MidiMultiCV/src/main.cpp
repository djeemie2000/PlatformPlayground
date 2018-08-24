#include <mbed.h>
#include "MidiParser.h"
#include "LogMidiHandler.h"
#include "MCP4x22.h"



int main() {
  DigitalOut ledOut(PC_13);//builtin led as gate indicator
  DigitalOut gateOut(PA_11);//gate output
  DigitalIn clockIn(PA_12);//clock input
  //Serial pc(PA_2, PA_3); // tx, rx
  Serial pc2(PA_9, PA_10); // tx, rx

  //Serial pcMidi(PB_10, PB_11); // tx, rx
  Serial pcMidi(PA_2, PA_3); // tx, rx


    pc2.baud(115200);
    wait_ms(4000);
    pc2.printf("\r\n-\r\n-\r\nMidi multi CV...\r\n-\r\n-\r\n");
    //wait_ms(1000);

    pcMidi.baud(31250);//midi baudrate

    // put your setup code here, to run once:
    MidiParser parser;
    Timer timer;
//    int counter = 0;
    LogMidiHandler midiHandler(pc2);

    while(1) {
        // put your main code here, to run repeatedly:
        timer.reset();
        timer.start();

        int numRead = 0;
        for(int repeat = 0; repeat<1000; ++repeat)
        {

          while(pcMidi.readable())
          {
            ++numRead;
            uint8_t byte = pcMidi.getc();
            pc2.printf("0x%2x ", byte);
            parser.Parse(byte, midiHandler);
          }
          //wait_ms(1);
        }
        // toggle led
        ledOut = !ledOut;

        //wait_ms(500);

        timer.stop();
        //pc2.printf("The time taken was %f seconds, count=%d numRead=%d \r\n", timer.read(), counter++, numRead);
    }
}
