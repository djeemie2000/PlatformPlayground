#include <Arduino.h>
#include "fastdigitalwrite.h"
#include "state.h"

#define USE_TIMER_1 true
#define USE_TIMER_2 false
#define USE_TIMER_3 false

#define DEBUGAPP 1

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
static const int GateOutPin = 2; // first gate output, other gate outputs ascending

Peripherals peripherals;

void printState(State& state, SerialOut& serial)
{
  serial.println("state:");
  serial.printf("bank %d slot %d editTrack %d editQ %d mode %d", state.bank, state.slot, state.editTrack, state.editQuadrant, state.mode);
}

void printSharedState(SharedState state, SerialOut& serial)
{
  serial.printf("step %d", state.currentStep);
  serial.println("current track:");
  serial.printf("playmute %x solo %x fill %x", state.currentPattern->playMute, state.currentPattern->solo, state.currentPattern->fill);
}

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
  if (clockIn && !interruptState.clockIn)
  {
    // advance current step
    sharedState.currentStep = (sharedState.currentStep + 1 < Pattern::NumSteps) ? sharedState.currentStep + 1 : 0;
  }

  if (sharedState.doReset)
  {
    sharedState.currentStep = 0;
    sharedState.doReset = false;
  }

  if (sharedState.doAdvance)
  {
    // advance current step
    sharedState.currentStep = (sharedState.currentStep + 1 < Pattern::NumSteps) ? sharedState.currentStep + 1 : 0;
    sharedState.doAdvance = false;
  }

  // update input state
  interruptState.clockIn = clockIn;
  interruptState.resetIn = resetIn;

  // set track outputs (currentStep, currentPattern, clock high/low)
  // gate outputs:
  //    start from current step of current pattern
  //      when fill => always gate
  //   take into account playmute
  //      when there is a solo, only play the solo track (solo mask bit = 1 => solo)
  uint8_t gateMask = sharedState.currentPattern->steps[sharedState.currentStep] | sharedState.currentPattern->fill;
  uint8_t playMask = sharedState.currentPattern->solo == 0x00 ? sharedState.currentPattern->playMute : sharedState.currentPattern->solo;
  // also take into account the clockOn/clockOff value!
  uint8_t legatoMask = interruptState.clockIn ? sharedState.currentPattern->clockOnValue : sharedState.currentPattern->clockOffValue;
  fastDigitalWritePortD<2>(gateMask & 0x01 & playMask & legatoMask); // gate 0
  fastDigitalWritePortD<3>(gateMask & 0x02 & playMask & legatoMask); // gate 1
  fastDigitalWritePortD<4>(gateMask & 0x04 & playMask & legatoMask); // gate 2
  fastDigitalWritePortD<5>(gateMask & 0x08 & playMask & legatoMask); // gate 3
  fastDigitalWritePortD<6>(gateMask & 0x10 & playMask & legatoMask); // gate 4
  fastDigitalWritePortD<7>(gateMask & 0x20 & playMask & legatoMask); // gate 5
  fastDigitalWritePortB<0>(gateMask & 0x40 & playMask & legatoMask); // gate 6
  fastDigitalWritePortB<1>(gateMask & 0x80 & playMask & legatoMask); // gate 7
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

  for (int idx = 0; idx < Pattern::NumTracks; ++idx)
  {
    pinMode(GateOutPin + idx, OUTPUT);
  }

  // setup I2C, SPI, peripherals
  // short pause before configuring the led matrix
  delay(1000);
  peripherals.serialOut.println("Setup led matrix...");
  peripherals.ledMatrix.Configure();
  delay(200);
  peripherals.serialOut.println("Setup touchpad...");
  peripherals.touchPad.Begin(TTP8229TouchPad::I2CMode);

  // run tests for UI here (ledMatrix, touchpad)
  TestTouchPad(peripherals.touchPad, peripherals.serialOut, 5);
  TestDigitalOutMatrix(peripherals.ledMatrix, peripherals.serialOut, 30);

  // TestDigitalOutMatrixFast(peripherals.ledMatrix, peripherals.serialOut, 200);
  // peripherals.serialOut.printf("State size %d", sizeof(loopState));

  // load params from EEPROM
  peripherals.serialOut.print("Loading params...");
  loadParams();
  sharedState.currentPattern = &(loopState.pattern[loopState.slot]);
  peripherals.serialOut.println(" done");

  // start timer with period 1 msec (this determines the resolution for the gate/clock handling)
  ITimer1.init();
  ITimer1.attachInterrupt(1000, oninterrupt);
}

void displayCurrentPattern()
{
  // show state in ui (ledmatrix)
  Pattern *pat = sharedState.currentPattern;
  int currentStep = sharedState.currentStep;
  for (int row = 0; row < Pattern::NumTracks; ++row)
  {
    for (int col = 0; col < Pattern::NumSteps; ++col)
    {
      int value = GetStep(pat, row, col);

      // indicate current (play) step by inverting that column
      if (col == currentStep && IsPlaying(pat, row))
      {
        value = 1 - value;
      }
      // set led
      if (value)
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
}

void displayTrackProperties()
{
  // display all track properties (for current pattern)
  // TODO indicate current edit track by blinking that row
  Pattern * pat = sharedState.currentPattern;
  for(int row = 0; row< Pattern::NumTracks; ++row)
  {
    int col = 0;
    if (bitRead(pat->clockOnValue, row))
    {
      peripherals.ledMatrix.Set(row, col++);
    }
    else
    {
      peripherals.ledMatrix.Clear(row, col++);
    }

    if (bitRead(pat->clockOffValue, row))
    {
      peripherals.ledMatrix.Set(row, col++);
    }
    else
    {
      peripherals.ledMatrix.Clear(row, col++);
    }

    if (bitRead(pat->gateTrigger, row))
    {
      peripherals.ledMatrix.Set(row, col++);
    }
    else
    {
      peripherals.ledMatrix.Clear(row, col++);
    }

    if (bitRead(pat->trackLink, row))
    {
      peripherals.ledMatrix.Set(row, col++);
    }
    else
    {
      peripherals.ledMatrix.Clear(row, col++);
    }

    if (bitRead(pat->trackProperty5, row))
    {
      peripherals.ledMatrix.Set(row, col++);
    }
    else
    {
      peripherals.ledMatrix.Clear(row, col++);
    }

    if (bitRead(pat->trackProperty6, row))
    {
      peripherals.ledMatrix.Set(row, col++);
    }
    else
    {
      peripherals.ledMatrix.Clear(row, col++);
    }

    if (bitRead(pat->trackProperty7, row))
    {
      peripherals.ledMatrix.Set(row, col++);
    }
    else
    {
      peripherals.ledMatrix.Clear(row, col++);
    }

    if (bitRead(pat->trackProperty8, row))
    {
      peripherals.ledMatrix.Set(row, col++);
    }
    else
    {
      peripherals.ledMatrix.Clear(row, col++);
    }

    for(; col<Pattern::NumSteps; ++col)
    {
      peripherals.ledMatrix.Clear(row, col);
    }
  }

  peripherals.ledMatrix.WriteAll();
}

void loop()
{
  // put your main code here, to run repeatedly:

  // read input (touchpad)
  peripherals.touchPad.Read();

  // M1 M2
  if (peripherals.touchPad.Get(0))
  {
    // edit mode, no track properties editing
    // set bit 0
    loopState.mode = bit(0);
  }
  if (peripherals.touchPad.Get(1))
  {
    // performance mode
    // set bit 1
    loopState.mode = bit(1);
  }
  // TODO utilities mode if both are pressed??

  if (peripherals.touchPad.Get(2))
  {
    // FA pressed
    bitSet(loopState.mode, 2);
  }
  else
  {
    // FA not pressed
    bitClear(loopState.mode, 2);
  }

  if (peripherals.touchPad.Get(3))
  {
    // FB pressed
    bitSet(loopState.mode, 3);
  }
  else
  {
    // FB not pressed
    bitClear(loopState.mode, 3);
  }

  if (bitRead(loopState.mode, 0))
  {
    // -- edit mode ---

    if (bitRead(loopState.mode, 2))
    {
      // FA pressed =>
      // select track + edit track properties 0-3
      for (int trk = 0; trk < 8; ++trk)
      {
        if (peripherals.touchPad.IsClicked(8 + trk))
        {
          loopState.editTrack = trk;
        }
      }
      // button F1-F4 (4-7) -> track properties
      if (peripherals.touchPad.IsClicked(4))
      {
        ToggleClockOnValue(sharedState.currentPattern, loopState.editTrack);
      }
      if (peripherals.touchPad.IsClicked(5))
      {
        ToggleClockOffValue(sharedState.currentPattern, loopState.editTrack);
      }
      if (peripherals.touchPad.IsClicked(6))
      {
        ToggleGateTrigger(sharedState.currentPattern, loopState.editTrack);
      }
      if (peripherals.touchPad.IsClicked(7))
      {
        // toggle link state!!
        // bit 0 -> or track 0 and 1
        // bit 1 -> chain track 0 and 1
        // bit 2 -> or track 2 and 3
        // bit 3 -> chain track 2 and 3
        // ...
        bitToggle(sharedState.currentPattern->trackLink, loopState.editTrack);
        if (loopState.editTrack % 2)
        {
          // editTrack odd => chaining toggled => if chaining 1 then or 0
          if (bitRead(sharedState.currentPattern->trackLink, loopState.editTrack))
          {
            bitClear(sharedState.currentPattern->trackLink, loopState.editTrack - 1);
          }
        }
        else
        {
          // editTrack even => or toggled => if or 1 then chaining 0
          if (bitRead(sharedState.currentPattern->trackLink, loopState.editTrack))
          {
            bitClear(sharedState.currentPattern->trackLink, loopState.editTrack + 1);
          }
        }
      }
    }
    else if (bitRead(loopState.mode, 3))
    {
      // FB pressed =>
      // select track + edit track properties 4-7
      // Button B1-B8 -> select edit track
      for (int trk = 0; trk < 8; ++trk)
      {
        if (peripherals.touchPad.IsClicked(8 + trk))
        {
          loopState.editTrack = trk;
        }
      }
      // button F1-F4 (4-7) -> toggle track properties
      if (peripherals.touchPad.IsClicked(4))
      {
        bitToggle(sharedState.currentPattern->trackProperty5, loopState.editTrack);
      }
      if (peripherals.touchPad.IsClicked(5))
      {
        bitToggle(sharedState.currentPattern->trackProperty6, loopState.editTrack);
      }
      if (peripherals.touchPad.IsClicked(6))
      {
        bitToggle(sharedState.currentPattern->trackProperty7, loopState.editTrack);
      }
      if (peripherals.touchPad.IsClicked(7))
      {
        bitToggle(sharedState.currentPattern->trackProperty8, loopState.editTrack);
      }
    }
    else
    {
      // FA and FB not pressed =>
      // edit steps
      // button F1-F4 (4-7) =>select quadrant
      for (int quad = 0; quad < 4; ++quad)
      {
        if (peripherals.touchPad.Get(4 + quad))
        {
          loopState.editQuadrant = quad;
        }
      }
      // Button B1-B8 (8-15) for steps
      for (int stp = 0; stp < 8; ++stp)
      {
        if (peripherals.touchPad.IsClicked(8 + stp))
        {
          int step = 8 * loopState.editQuadrant + stp;
          // peripherals.serialOut.printf("Toggle track %d step %d", loopState.editTrack, step);
          // toggle on/off ~editTrack, editQuadrant, stp
          ToggleStep(sharedState.currentPattern, loopState.editTrack, step);
        }
      }
    }
  }
  else
  {
    // --- performance mode ---
    if(peripherals.touchPad.Get(2))
    {
      // FA => save all to current bank
      peripherals.serialOut.print("Saving params...");
      saveParams();
      peripherals.serialOut.println(" done");
    }

    // any Button 1-8 clicked => action depending on whcih function key (FA) FB F1-F4 is pressed 
    for (int btn = 0; btn < 8; ++btn)
    {
      if (peripherals.touchPad.IsClicked(8 + btn))
      {
        if(peripherals.touchPad.Get(3))
        {
          // jump to slot stp
          loopState.slot = btn;
          sharedState.currentPattern = &(loopState.pattern[loopState.slot]);
        }
        else if(peripherals.touchPad.Get(4))
        {
          // F1
          TogglePlayMute(sharedState.currentPattern, btn);
        }
        else if(peripherals.touchPad.Get(5))
        {
          // F2 => solo
          ToggleSolo(sharedState.currentPattern, btn);
        }
        else if(peripherals.touchPad.Get(6))
        {
          // F3 => toggle fill
          ToggleFill(sharedState.currentPattern, btn);
        }
        else if(peripherals.touchPad.Get(7))
        {
          // F4 => toggle play property 4
          bitToggle(sharedState.currentPattern->playProperty4, btn);
        }
      }
    }
  }

  //   // play related controls: reset / advance / ..
  //   if(peripherals.touchPad.IsClicked(8))
  //   {
  //     //Serial.println("reset");
  //     // trigger a reset in the interrupt
  //     sharedState.doReset = true;
  //   }
  //   if(peripherals.touchPad.IsClicked(9))
  //   {
  //     //Serial.println("advance");
  //     // trigger an advance in the interrupt
  //     sharedState.doAdvance = true;
  //   }
  // }
  // else if(peripherals.touchPad.Get(14))
  // {
  //   // toggle slot
  //   // toggle bank -> not enough memory, perhaps later load bank from memorybank storage?
  //   for(int trk = 0 ; trk<8; ++trk)
  //   {
  //     if(peripherals.touchPad.IsClicked(trk))
  //     {
  //       loopState.slot = trk;
  //       sharedState.currentPattern = &(loopState.pattern[loopState.slot]);
  //     }
  //   }
  // }

  // check if edit mode and FA or FB pressed
  if(loopState.mode == 0x05 || loopState.mode == 0x09)
  {
    displayTrackProperties();
  }
  else
  {
    // show current pattern (on led matrix) 
    displayCurrentPattern();
  }

#ifdef DEBUGAPP
  // debug/test code:
  // Serial.print(interruptState.clockIn);
  // Serial.print(interruptState.resetIn);
  // Serial.println(sharedState.currentStep);
  printState(loopState, peripherals.serialOut);
  printSharedState(sharedState, peripherals.serialOut);
  delay(125);
#endif
}
