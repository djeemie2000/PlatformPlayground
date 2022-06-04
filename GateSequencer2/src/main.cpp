#include <Arduino.h>
#include "fastdigitalwrite.h"
#include "state.h"

#define USE_TIMER_1     true
#define USE_TIMER_2     false
#define USE_TIMER_3     false

#include "TimerInterrupt.h"
#include <Wire.h>
#include "peripherals.h"
#include <EEPROM.h>

// global variables
State loopState;
SharedState sharedState;
InterruptState interruptState;

// pins
static const int ClockInPin = A0;
static const int ResetInPin = A1;
static const int GateOutPin = 2;//first gate output, other gate outputs ascending


Peripherals peripherals;

void saveParams()
{
  int off = 0;
   // header
  EEPROM.update(off++, 'G');
  EEPROM.update(off++, '8');
  uint8_t version = 0x01;
  EEPROM.update(off++, version);
  // state
  EEPROM.put(off, loopState);
  off += sizeof(State);
  // 
}

void loadParams()
{
  int off = 0;
  // header
  if (EEPROM.read(off++) == 'G' && EEPROM.read(off++) == '8')
  {
    uint8_t version = EEPROM.read(off++);
    if (version == 0x01)
    {
      // state
      EEPROM.get(off, loopState);
    }
  }
}

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
    // gate outputs
    // ~ current step of current pattern and ~playmute!
    uint8_t gateMask = sharedState.currentPattern->steps[sharedState.currentStep];
    uint8_t playMask = sharedState.currentPattern->playMute;
    fastDigitalWritePortD<2>( gateMask & 0x01 & playMask); // gate 0
    fastDigitalWritePortD<3>( gateMask & 0x02 & playMask); // gate 1
    fastDigitalWritePortD<4>( gateMask & 0x04 & playMask); // gate 2
    fastDigitalWritePortD<5>( gateMask & 0x08 & playMask); // gate 3
    fastDigitalWritePortD<6>( gateMask & 0x10 & playMask); // gate 4
    fastDigitalWritePortD<7>( gateMask & 0x20 & playMask); // gate 5
    fastDigitalWritePortB<0>( gateMask & 0x40 & playMask); // gate 6
    fastDigitalWritePortB<1>( gateMask & 0x80 & playMask); // gate 7
  }
  else
  {
    // only legato gate outputs while clock is off
    // ~ current step of current pattern and ~playmute!
    uint8_t gateMask = sharedState.currentPattern->steps[sharedState.currentStep];
    uint8_t playMask = sharedState.currentPattern->playMute;
    uint8_t legatoMask = sharedState.currentPattern->legato;
    fastDigitalWritePortD<2>( gateMask & 0x01 & playMask & legatoMask); // gate 0
    fastDigitalWritePortD<3>( gateMask & 0x02 & playMask & legatoMask); // gate 1
    fastDigitalWritePortD<4>( gateMask & 0x04 & playMask & legatoMask); // gate 2
    fastDigitalWritePortD<5>( gateMask & 0x08 & playMask & legatoMask); // gate 3
    fastDigitalWritePortD<6>( gateMask & 0x10 & playMask & legatoMask); // gate 4
    fastDigitalWritePortD<7>( gateMask & 0x20 & playMask & legatoMask); // gate 5
    fastDigitalWritePortB<0>( gateMask & 0x40 & playMask & legatoMask); // gate 6
    fastDigitalWritePortB<1>( gateMask & 0x80 & playMask & legatoMask); // gate 7
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
  peripherals.touchPad.Begin(TTP8229TouchPad::I2CMode);

  // run tests for UI here (ledMatrix, touchpad)
  //TestTouchPad(peripherals.touchPad, peripherals.serialOut, 10);
  TestDigitalOutMatrix(peripherals.ledMatrix, peripherals.serialOut, 50);
  //peripherals.serialOut.printf("State size %d", sizeof(loopState));

  // load params from EEPROM
  peripherals.serialOut.print("Loading params...");
  loadParams();
  sharedState.currentPattern = &(loopState.pattern[loopState.slot]);
  peripherals.serialOut.println(" done");
    
  // start timer with period 1 msec (this determines the resolution for the gate/clock handling)
  ITimer1.init();
  ITimer1.attachInterrupt(1000, oninterrupt); 
}


void loop() 
{
  // put your main code here, to run repeatedly:  
  
  // read input (touchpad)
  peripherals.touchPad.Read();

  // update state according to input
  // including toggle play/mute
  // including change slot/currentPattern
  if(peripherals.touchPad.Get(12))
  {
    // select edit track mode
    for(int trk = 0 ; trk<8; ++trk)
    {
      if(peripherals.touchPad.IsClicked(trk))
      {
         loopState.editTrack = trk;
      }
    }
    if(peripherals.touchPad.IsClicked(8))
    {
      // toggle legato on/off on edit track
      ToggleLegato(sharedState.currentPattern, loopState.editTrack);
    }
    // perhaps other track related parameters could be set here?
  }
  else if(peripherals.touchPad.Get(13))
  {
    // toggle play/mute on track
    for(int trk = 0 ; trk<8; ++trk)
    {
      if(peripherals.touchPad.IsClicked(trk))
      {
         TogglePlayMute(sharedState.currentPattern, trk);
      }
    }
  }
  else if(peripherals.touchPad.Get(14))
  {
    // toggle slot 
    // toggle bank -> not enough memory, perhaps later load bank from memorybank storage?
    for(int trk = 0 ; trk<8; ++trk)
    {
      if(peripherals.touchPad.IsClicked(trk))
      {
        loopState.slot = trk;
        sharedState.currentPattern = &(loopState.pattern[loopState.slot]);
      }
    }
  }
  else
  {
    // (default) toggle mode
    for(int quad = 0; quad<4; ++quad)
    {
      if(peripherals.touchPad.Get(8+quad))
      {
        loopState.editQuadrant = quad;
      }
    }
    for(int stp = 0 ; stp<8; ++stp)
    {
      if(peripherals.touchPad.IsClicked(stp))
      {
        int step =  8*loopState.editQuadrant + stp;
        //peripherals.serialOut.printf("Toggle track %d step %d", loopState.editTrack, step);
        //toggle on/off ~editTrack, editQuadrant, stp
        Toggle(sharedState.currentPattern, loopState.editTrack, step);
      }
    }
  } 

  if(peripherals.touchPad.IsClicked(15))
  {
    peripherals.serialOut.print("Saving params...");
    saveParams();
    peripherals.serialOut.println(" done");
  }

  // show state in ui (ledmatrix)
  Pattern* pat = sharedState.currentPattern;
  int currentStep = sharedState.currentStep;
  for(int row = 0; row<Pattern::NumTracks; ++row)
  {
    for(int col = 0; col<Pattern::NumSteps; ++col)
    {
      int value = Get(pat,row, col);
      
      // indicate current (play) step by inverting that column
      if(col == currentStep && IsPlaying(pat, row))
      {
        value = 1-value;
      }
      // set led
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
