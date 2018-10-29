#include <mbed.h>
#include "GateIn.h"
#include "GateTrack.h"
#include "LogMidiHandler.h"

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

  // common
  GateIn clockIn(PA_12);//external clock input
  LogMidiHandler midiLog(pc2, 1);
  GateIn muteBtn(A1);
  GateIn setBtn(A2);
  GateIn clearBtn(A5);
  ToggleIn learnModeBtn(A6);
  //TODO play/Step mode toggle btn
  //TODO reset/advance btn

  // track
  //TODO multiple tracks
  GateIn trackBtn(A0);
  GateTrack track(midiLog);
  const uint8_t MidiNote = 0x40;
  const uint8_t MidiChannel = 0x04;
  track.Learn(MidiNote, MidiChannel);

  // 
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
        muteBtn.Read();
        setBtn.Read();
        clearBtn.Read();
        learnModeBtn.Read();
        //
        bool mutePressed = muteBtn.Get();
        bool setPressed = setBtn.Get();
        bool clearPressed = clearBtn.Get();
        bool learnMode = learnModeBtn.Get();

        trackBtn.Read();
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
            else if(clearPressed)
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
          track.StepOn();
        }
        else if(clockIn.IsFalling())
        {
          // step off
          track.StepOff();
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