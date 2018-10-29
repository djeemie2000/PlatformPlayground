#include <mbed.h>
#include "GateIn.h"
#include "GateTrack.h"

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
  pc2.printf("version 0.1\r\n");
  wait_ms(1000);

  GateIn clockIn(PA_12);//external clock input

  GateIn trackBtn(A0);//TODO multiple
  GateTrack track;//TODO multiple
  track.Learn(0x40, 0x04);

  Timer timer;
  int counter = 0;

  wait_ms(500);
  pc2.printf("start processing\r\n");
  while(1) {
    // put your main code here, to run repeatedly:
      timer.reset();
      timer.start();

      for(int repeat = 0; repeat<1000; ++repeat)
      {
        //
        bool mutePressed = repeat<100;
        bool setPressed = 100<repeat && repeat<200;
        bool clearPressed = 200<repeat && repeat<300;
        bool learnMode = 300<repeat;

        trackBtn.read();
        if(trackBtn.IsRising())
        {
            if(mutePressed)
            {
              //toggle mute
              track.Mute(!track.IsMuted());
            } 
            else if(setPressed)
            {
              //set current Step
              track.SetCurrentStep();
            } 
            else if(clearStep)
            {
              //clear current step
              track.ClearCurrentStep();
            }
            else if(learnMode)
            {
              //learn ~ pot analog in
              //TODO
            }
            else
            {
              //play note on
              track.PlayOn();
            }
        }
        else if(trackBtn.IsFalling())
        {
          //play note off (if note is on)
          track.PlayOff();
        }

        // 
        clockIn.Read();
        if(clockIn.IsRising())
        {
          // step on
        }
        else if(clockIn.IsFalling())
        {
          // step off
        }

        //
        ledOut = repeat<500 ? 0:1;//inverted
        // 
        wait_ms(1);
      }

      timer.stop();
      pc2.printf("%d : %d ms\r\n", counter++, timer.read_ms());

  }
}