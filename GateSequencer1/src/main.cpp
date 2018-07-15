//#include "stm32f103c8t6.h"
#include <mbed.h>

const PinName ledPin = PC_13;
DigitalOut ledOut(ledPin);//builtin led as gate indicator
DigitalOut gateOut(PA_11);//gate output
//Serial pc(PA_2, PA_3); // tx, rx
Serial pc2(PA_9, PA_10); // tx, rx

// step gates / leds output
BusOut stepOut(PA_12, PA_15, PB_3, PB_4, PB_5, PB_6, PB_7, PB_8);
AnalogIn CV0(PA_0);
AnalogIn CV1(PA_1);
AnalogIn CV2(PA_2);
AnalogIn CV3(PA_3);
AnalogIn CV4(PA_4);
AnalogIn CV5(PA_5);
AnalogIn CV6(PA_6);
AnalogIn CV7(PA_7);
AnalogIn CV8(PB_0);
AnalogIn CV9(PB_1);

int counter;

struct Sequencer {
  uint32_t counter;
  uint32_t clockPeriod;
  uint32_t stepDuration;//[0,256[

  static const int numSteps = 8;
  int currentStep;
  int lowStep;
  int highStep;

  static const int numCVs = 10;
  AnalogIn* CVin[numCVs];
  int CVvalue[numCVs];
  int currentCV;

  Sequencer()
  : counter(0), clockPeriod(500), stepDuration(32)
  , currentStep(0), lowStep(0), highStep(numSteps-1)
  , currentCV(0){
    CVin[0] = &CV0;
    CVin[1] = &CV1;
    CVin[2] = &CV2;
    CVin[3] = &CV3;
    CVin[4] = &CV4;
    CVin[5] = &CV5;
    CVin[6] = &CV6;
    CVin[7] = &CV7;
    CVin[8] = &CV8;
    CVin[9] = &CV9;

    for(int idx = 0; idx<numCVs; ++idx) {
      CVvalue[idx] = 512-idx*50;
    }
  }

  void tick() {
    ++counter;
    if(clockPeriod<=counter) {
        endStep();
        advanceStep();
        beginStep();
    }
    readCV();
    setGate();
  }

  void readCV() {
    CVvalue[currentCV] = CVin[currentCV]->read_u16();
    currentCV = (currentCV+1)%numCVs;
  }

  void setGate() {
    // use CV values
    uint32_t currStepDuration = (1+(CVvalue[currentStep]>>7))>>1;//scale 2^16 to 256, rounding to nearest
    //uint32_t currStepDuration = (numSteps-currentStep)*32;//scale to 256

    bool gate = counter*256u < currStepDuration*clockPeriod;
    gateOut = gate?1:0;
    ledOut = gate?0:1;

    // step led follows gate
    if(gate) {
      stepOut.write(1<<currentStep);
    } else {
      stepOut.write(0);
    }

  }

  void endStep() {
    //update clock period
    clockPeriod = (clockPeriod+counter)/2;
  }

  void beginStep(){
    counter = 0;
    //stepOut.write(1<<currentStep);
  }

  void advanceStep() {
    if(lowStep<=highStep) {
      //advance
      currentStep += 1;
      if(highStep<currentStep) {
        currentStep = lowStep;
      }
    } else {
      //inverse advance
      currentStep -= 1;
      if(currentStep<lowStep) {
        currentStep = highStep;
      }
    }
  }
};

int main() {

    // put your setup code here, to run once:
    //pc.baud(115200);
    //pc.printf("BlinkLed...");
    pc2.baud(115200);
    wait_ms(4000);
    pc2.printf("Gate Sequencer...");
    //pc3.baud(115200);
    //spc3.printf("BlinkLed...");
    counter = 0;


    Sequencer sequencer;
    Timer timer;
    // osStatus err = thread.start(callback(ledThread));
    // if(err) {
    //   pc2.printf("error is %d\r\n", (int)err);
    // }

    // osStatus err2 = thread2.start(callback(printThread));
    // if(!err2) {
    //   pc2.printf("no error is %d\r\n", (int)err2);
    // }
    // if(err2) {
    //   pc2.printf("error is %d\r\n", (int)err2);
    // }



    while(true) {
        //put your main code here, to run repeatedly:

        timer.reset();
        timer.start();
        for(int repeat = 0; repeat<800; ++ repeat) {
          sequencer.tick();
          wait_us(300);
        }
        timer.stop();

        pc2.printf("The time taken was %f seconds\n", timer.read());
        pc2.printf("counter is %d\r\n", counter++);

        pc2.printf("Sequencer: step : curr %d lo %d hi %d\r\n", sequencer.currentStep, sequencer.lowStep, sequencer.highStep);

        for(int idx = 0; idx<8; ++idx) {
          pc2.printf("CV %d : %d\r\n", idx, sequencer.CVvalue[idx]);
        }

       //    ledOn();
       //    wait_ms(250);
       //    ledOff();
       //    wait_ms(250);
       //
       // wait_ms(1000);
    }
}
