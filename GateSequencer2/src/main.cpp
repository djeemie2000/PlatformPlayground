#include <Arduino.h>
#include "fastdigitalwrite.h"
#include "state.h"

#define USE_TIMER_1     true
#define USE_TIMER_2     false
#define USE_TIMER_3     false

#include "TimerInterrupt.h"
#include <Wire.h>
#include "SerialOut.h"
#include "Max7219Matrix.h"
#include "TestDigitalOutMatrix.h"

// global variables
State loopState;
SharedState sharedState;
InterruptState interruptState;

// pins
static const int ClockInPin = A0;
static const int ResetInPin = A1;
static const int GateOutPin = 2;//first gate output, other gate outputs ascending

// peripherals
struct Peripherals
{
  SerialOut serialOut;
  Max7219Matrix ledMatrix;
  
  Peripherals()
   :serialOut()
   , ledMatrix(4, PIN_SPI_SS)
  {}
};

Peripherals peripherals;

void oninterrupt()
{
  // read clock/reset inputs
  int clockIn = fastDigitalReadPortC<0>();
  int resetIn = fastDigitalReadPortC<1>();

  // handle rising inputs (update currentStep)
  if(clockIn && !interruptState.clockIn)
  {
    //advance current step
    sharedState.currentStep = (sharedState.currentStep+1<Pattern::NumSteps) ? sharedState.currentStep + 1 : 0;
  }
  
  // update input state
  interruptState.clockIn = clockIn;
  interruptState.resetIn = resetIn;

  // set track outputs (currentStep, currentPattern, clock high/low)
  if(interruptState.clockIn)
  {
    // gate outputs ~ current step of current pattern
    // TODO playmute!
    uint8_t gateMask = sharedState.currentPattern->steps[sharedState.currentStep];
    fastDigitalWritePortD<2>( gateMask & 0x01 ); // gate 0
    fastDigitalWritePortD<3>( gateMask & 0x02 ); // gate 1
    fastDigitalWritePortD<4>( gateMask & 0x04 ); // gate 2
    fastDigitalWritePortD<5>( gateMask & 0x08 ); // gate 3
    fastDigitalWritePortD<6>( gateMask & 0x10 ); // gate 4
    fastDigitalWritePortD<7>( gateMask & 0x20 ); // gate 5
    fastDigitalWritePortB<0>( gateMask & 0x40 ); // gate 6
    fastDigitalWritePortB<1>( gateMask & 0x80 ); // gate 7
  }
  else
  {
    // gate outputs all to zero
    fastDigitalWritePortD<2>(0); // gate 0
    fastDigitalWritePortD<3>(0); // gate 1
    fastDigitalWritePortD<4>(0); // gate 2
    fastDigitalWritePortD<5>(0); // gate 3
    fastDigitalWritePortD<6>(0); // gate 4
    fastDigitalWritePortD<7>(0); // gate 5
    fastDigitalWritePortB<0>(0); // gate 6
    fastDigitalWritePortB<1>(0); // gate 7
  }
}

void setup() 
{
  // put your setup code here, to run once:

   // serial port for debugging
  Serial.begin(115200);
  Serial.println("GateSequencer2...");

  sharedState.currentPattern = &(loopState.pattern[0]);

  // setup inputs and outputs
  pinMode(ClockInPin, INPUT_PULLUP);
  pinMode(ResetInPin, INPUT_PULLUP);
  
  for(int idx = 0; idx<Pattern::NumTracks; ++idx)
  {
    pinMode(GateOutPin + idx, OUTPUT);
  }

  // setup I2C, SPI, peripherals
  peripherals.ledMatrix.Configure();

  //TODO run tests for UI here (ledMatrix, touchpad)
  TestDigitalOutMatrix(peripherals.ledMatrix, peripherals.serialOut, 100);

  // start timer with period 1 msec (this determines the resolution for the gate/clock handling)
  ITimer1.init();
  ITimer1.attachInterrupt(1000, oninterrupt); 
}

void loop() 
{
  // put your main code here, to run repeatedly:
  
  // read input (touchpad)
  // update state according to input
  
  // show state in ui (ledmatrix)
  Pattern* pat = sharedState.currentPattern;
  int currentStep = sharedState.currentStep;
  for(int row = 0; row<Pattern::NumTracks; ++row)
  {
    for(int col = 0; col<Pattern::NumSteps; ++col)
    {
      int value = (pat->steps[col] >> row) & 1;// 1 or 0
      // indicate current (play) setp by inverting that column
      if(col == currentStep)
      {
        value = 1-value;
      }
      if(value)
      {
        peripherals.ledMatrix.Set(row, col);        
      }
      else
      {
        peripherals.ledMatrix.Clear(row, col);        
      }
    } 
  }
  peripherals.ledMatrix.WriteAll();

  // debug/test code:
  // Serial.print(interruptState.clockIn);
  // Serial.print(interruptState.resetIn);
  // Serial.println(sharedState.currentStep);
  // delay(250);
}
