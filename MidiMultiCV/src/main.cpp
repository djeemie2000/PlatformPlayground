#include <mbed.h>
#include "MidiParser.h"

DigitalOut ledOut(PC_13);//builtin led as gate indicator
DigitalOut gateOut(PA_11);//gate output
DigitalIn clockIn(PA_12);//clock input
//Serial pc(PA_2, PA_3); // tx, rx
Serial pc2(PA_9, PA_10); // tx, rx

Serial pcMidi(PB_10, PB_11); // tx, rx


int main() {

    // put your setup code here, to run once:
    MidiParser parser;
    Timer timer;
    int counter = 0;


    while(1) {
        // put your main code here, to run repeatedly:
        timer.reset();
        timer.start();

        while(pcMidi.readable())
        {
          uint8_t byte = pcMidi.getc();
          parser.Parse(byte);

          pc2.printf("0x%2x ", byte);
        }

        wait_ms(1000);

        timer.stop();
        pc2.printf("The time taken was %f seconds, count=%d \n", timer.read(), counter++);


    }
}
