#pragma once
#include <Arduino.h>

#include "delayline.h"
#include "mcpdac.h"
#include "fastdigitalwrite.h"
#include "lowpassfilter.h"
#include "pseudorandom.h"
#include "degrade.h"
#include "analoginbank.h"
#include "fastdigitalinbank.h"
#include "fastdigitaloutbank.h"
#include "analoginbank821.h"

#define DOSERIALDEBUG 1

// pins 10 11 12 13 for SPI
// TODO pins 4..9 for 6x digital in/out
// TODO pins 2 3 for 2x digital in
// TODO A6 A7 for 2x audio in (A7 unused if mono in)
// TODO A0..A5 for analog in 
// -> TODO use 4051 3 digital in + 1 analog in for 8 pots

struct NanoLooperApp
{
    static const int loopLength = 1024 + 512 + 128; // 1.625 kB = 7/4 kB
    // button/gate in pins
    static const int recordingButtonInPin = 2; // PD2
    static const int reverseInPin = 3;        // PD3
    static const int resetInPin = 4;          // PD4
    // degrade saturate non destructive via analogin
    // static const int degradeInPin = 5;         // PD5 
    // static const int saturateInPin = 6;       // PD6 
    static const int flippedInPin = 7;
    // output pins
    static const int squareLFOOutPin = 8; // PB0
    static const int randomOutPin = 9;  // BP1
    // pins 10 11 12 13 for SPI
    // analog in pins (8 to 1) A0 A1 A2 A3
    //TODO A4 A5
    static const int audioPinIn = A6; // ADC6
    static const int audioRightInPin = A7;//??? ADC7


    DelayLine<loopLength> delayLine; // assumes 328 2k memory
    MCPDac dac;                     // assumes arduino nano
    FastDigitalInBank<6> digitalIn; // assumes arduino nano
    FastDigitalOutBank<3> digitalOut; // assumes arduino nano
    AnalogInBank821<0,1,2> analogIn; // assumes arduino nano


    // random out (1/0)
    PseudoRandom pseudoRandom;
    int randomState;
    uint16_t randomThreshold;

    int delayusec;
    int resetOffset;

    uint8_t degrade;
    uint8_t saturate;

    // square LFO out (1/0)
    int squareLFOCounter;
    int squareLFOPeriod;
    int squareLFOState;

    NanoLooperApp()
    {
    }

    void Begin()
    {
        delayLine.Begin();
        dac.Begin();
        // button/gate in
        digitalIn.Assign(0, recordingButtonInPin, true);
        digitalIn.Assign(1, reverseInPin, true);
        digitalIn.Assign(2, resetInPin, true);
        // digitalIn.Assign(3, degradeInPin, true);
        // digitalIn.Assign(4, saturateInPin, true);
        digitalIn.Assign(5, flippedInPin, true);
        // outputs
        digitalOut.Assign(0, squareLFOOutPin);
        digitalOut.Assign(1, randomOutPin);

        analogIn.Begin(A3);

        delayusec = 0;
        resetOffset = 0;

        degrade = 0;
        saturate = 0;

        squareLFOCounter = 0;
        squareLFOPeriod = 16000;
        randomState = 1;
        randomThreshold = 128;
        squareLFOState = 1;

        analogIn.UpdateAll();
    }

    bool RecordingBtnClicked()
    {
        return digitalIn.IsFalling(0);
    }

    bool ResetGateRising()
    {
        return digitalIn.IsRising(2);
    }

    void update()
    {
        // first read record button in
        digitalIn.Update();
        if (RecordingBtnClicked())
        {
            DoFill();
            //DoRecording();
        }
        else
        {
            DoLooping();
        }
    }

    void DoLooping()
    {
        // first read CV(s)
        // alternate between CV ins 
        analogIn.Update();

        delayusec = analogIn.Get(0) >> 1;   // [0-512[ microseconds        
        resetOffset = analogIn.Get(1); // [0,1024[ TODO 
        squareLFOPeriod = analogIn.Get(2) << 3; // [0-32768/4[
        squareLFOPeriod = max(32, squareLFOPeriod);        
        randomThreshold = analogIn.Get(3); // [0, 1024[ ??

        degrade = analogIn.Get(4) >> 3;// [0,128[
        saturate = analogIn.Get(5) >> 3;// [0,128[

        // update square LFO
        if (squareLFOPeriod < squareLFOCounter)
        {            
            squareLFOCounter = 0;
            squareLFOState = 1 - squareLFOState;
#ifdef DOSERIALDEBUG
            Serial.println(squareLFOPeriod);
#endif
        }
        ++squareLFOCounter;
        digitalOut.Set(0, squareLFOState);

        // update random out
        uint32_t random = pseudoRandom.Rand();
        // compare with threshold ~ CV/pot
        if ((random & 0xFFFF) < randomThreshold)
        {
            // toggle state
            randomState = 1 - randomState;
        }
        digitalOut.Set(1, randomState);

        // reset gate rising
        if(digitalIn.IsRising(2))
        {
            // reset position from analogIn!!
            delayLine.Reset(resetOffset);
#ifdef DOSERIALDEBUG
            Serial.println(resetOffset);
#endif
        }

        // read audio from delay line
        uint8_t audioOut = delayLine.Read();    

        audioOut = Degrade(audioOut, degrade);
        audioOut = Saturate(audioOut, saturate);

        // write audio to fast dac out
        if(digitalIn.Get(5))
        {
            dac.Write(audioOut);
            dac.WriteR(audioOut);
        }
        else
        {
            dac.WriteFlipped(audioOut);
            dac.WriteRFlipped(audioOut);
        }

        // advance/reverse delay line
        if (!digitalIn.Get(1))
        {
            delayLine.Reverse();
        }
        else
        {
            delayLine.Advance();
        }

        digitalOut.Update();

        // delay ~ speed cv
        delayMicroseconds(delayusec);
    }

    void DoRecording()
    {
#ifdef DOSERIALDEBUG
        Serial.println('R');
#endif

        // always sample at highest freq

        // reset delay line to zero!
        delayLine.Reset(0);
        // reset squareLFO
        squareLFOCounter = 0;
        // do not reset random 

        int cntr = 0;
        while (cntr < loopLength)
        {
            // read audio in
            uint8_t audioValue = analogRead(audioPinIn) >> 2; // [0,256[

            digitalIn.Update();//??
            
            //  write delay line
            delayLine.Write(audioValue);

            // write to dac out for timing purposes (?)
            dac.Write(0x7F);
            dac.WriteR(0x7F);

            // advance delay line
            delayLine.Advance();

            // delay needed??
            // delayMicroseconds(delayusec);

            ++cntr;
        }

#ifdef DOSERIALDEBUG
        Serial.println('r');
#endif
    }

    void DoFill()
    {
#ifdef DOSERIALDEBUG
        Serial.println('F');
#endif

        // always sample at highest freq

        // reset delay line to zero!
        delayLine.Reset(0);
        // reset squareLFO
        squareLFOCounter = 0;
        // do not reset random 

        uint8_t audioValue = 0x00;
        int cntr = 0;
        while (cntr < loopLength)
        {
            // read audio in
            audioValue += 9; // [0,256[

            digitalIn.Update();//??
            
            //  write delay line
            delayLine.Write(audioValue);

            // write to dac out for timing purposes (?)
            dac.Write(0x7F);
            dac.WriteR(0x7F);

            // advance delay line
            delayLine.Advance();

            // delay needed??
            // delayMicroseconds(delayusec);

            ++cntr;
        }

#ifdef DOSERIALDEBUG
        Serial.println('f');
#endif
    }

};
