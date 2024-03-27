#pragma once
#include <Arduino.h>

#include "devboard.h"

#include "delayline.h"
//#include "lowpassfilter.h"
#include "pseudorandom.h"
#include "degrade.h"
#include "tapehead.h"
#include "squarelfostate.h"
#include "pseudorandomstate.h"

#define DOSERIALDEBUG 1

struct NanoLooperApp
{
    static const int loopLength = 1024 + 512;;// + 128; // 1.625 kB = 7/4 kB

    // button/gate in pins
    static const int recordingButtonInPin = 2; // PD2
    static const int reverseInPin = 3;        // PD3
    static const int resetInPin = 4;          // PD4
    static const int flippedInPin = 5;
    // TODO octaveUpPin 
    // degrade saturate non destructive via analogin
    // output pins
    static const int randomOutPin2 = 6;
    static const int squareLFOOutPin2 = 7; 
    static const int squareLFOOutPin = 8; // PB0
    static const int randomOutPin = 9;  // BP1
    // pins 10 11 12 13 for SPI
    // analog in pins (8 to 1) A0 A1 A2 A3
    //TODO A4 A5
    static const int audioPinIn = A6; // ADC6
    static const int audioRightInPin = A7;// ADC7

    DevBoard devBoard;//TODO 

    DelayLine<loopLength> delayLine; // assumes 328 2k memory
    TapeHead<loopLength> writeHead;
    TapeHead<loopLength> readHeadL;
    TapeHead<loopLength> readHeadR;

    int delayusec;
    int resetOffsetL;
    int resetOffsetR;

    uint8_t degrade;
    uint8_t saturate;

    // square LFO out (1/0)
    SquareLFOState<2> lfo;
    // random out (1/0)
    PseudoRandomState<2> pseudoRandom;

    NanoLooperApp()
    {
    }

    void Begin()
    {
        delayLine.Begin();
        writeHead.Reset(0);
        readHeadL.Reset(0);
        readHeadR.Reset(0);

        devBoard.Begin();

        delayusec = 0;
        resetOffsetL = 0;
        resetOffsetR = 32;

        degrade = 0;
        saturate = 0;

        lfo.Begin();
        lfo.SetPeriod(0, 8000);
        lfo.SetPeriod(1, 4000);
        
        pseudoRandom.Begin();
        pseudoRandom.SetThreshold(0, 128);
        pseudoRandom.SetThreshold(1, 128);

        devBoard.analogIn.UpdateAll();
    }

    bool RecordingBtnClicked()
    {
        return devBoard.gateButtonIn.IsFalling(0);
    }

    bool ResetGateRising()
    {
        return devBoard.gateButtonIn.IsRising(2);
    }

    void update()
    {
        // first read record button in
        devBoard.gateButtonIn.Update();
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
        devBoard.analogIn.Update();

        delayusec = devBoard.analogIn.Get(0) >> 1;   // [0-512[ microseconds        
        resetOffsetL = devBoard.analogIn.Get(1); // [0,1024[ TODO 
        resetOffsetR = devBoard.analogIn.Get(2); // [0,1024[ TODO 
        
        degrade = devBoard.analogIn.Get(6) >> 3;// [0,128[
        saturate = devBoard.analogIn.Get(7) >> 3;// [0,128[

        // update square LFO
        int squareLFOPeriod = devBoard.analogIn.Get(3) << 3; // [0-32768/4[
        squareLFOPeriod = max(32, squareLFOPeriod);        
        lfo.SetPeriod(0, squareLFOPeriod);
        lfo.SetPeriod(1, squareLFOPeriod/2);   
        lfo.Update();
        devBoard.gateOut.Set(0, lfo.Get(0));
        devBoard.gateOut.Set(2, lfo.Get(1));

        // update random out
        pseudoRandom.SetThreshold(0, devBoard.analogIn.Get(4));// [0, 1024[ ??
        pseudoRandom.SetThreshold(1, devBoard.analogIn.Get(4)/2);
        pseudoRandom.Update();
        devBoard.gateOut.Set(1, pseudoRandom.Get(0));
        devBoard.gateOut.Set(3, pseudoRandom.Get(1));

        // reset gate rising
        if(devBoard.gateButtonIn.IsRising(2))
        {
            // reset position is read from analogIn
            readHeadL.Reset(resetOffsetL);
            readHeadR.Reset(resetOffsetR);
#ifdef DOSERIALDEBUG
            Serial.println(resetOffsetL);
            Serial.println(resetOffsetR);
#endif
        }

        // read audio from delay line
        uint8_t audioOutL = delayLine.Read(readHeadL.Get());    
        audioOutL = Degrade(audioOutL, degrade);
        audioOutL = Saturate(audioOutL, saturate);

        uint8_t audioOutR = delayLine.Read(readHeadR.Get());    
        audioOutR = Degrade(audioOutR, degrade);
        audioOutR = Saturate(audioOutR, saturate);

        // write audio to fast dac out
        if(devBoard.gateButtonIn.Get(3))
        {
            devBoard.dac.Write(audioOutL);
            devBoard.dac.WriteR(audioOutR);
        }
        else
        {
            devBoard.dac.WriteFlipped(audioOutL);
            devBoard.dac.WriteRFlipped(audioOutR);
        }

        // advance/reverse delay line
        if (!devBoard.gateButtonIn.Get(1))
        {
            readHeadL.Reverse();
            readHeadR.Reverse();
        }
        else
        {
            readHeadL.Advance();
            readHeadR.Advance();
        }

        devBoard.gateOut.Update();

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
        //delayLine.Reset(0);
        writeHead.Reset(0);
        readHeadL.Reset(resetOffsetL);
        readHeadR.Reset(resetOffsetR);
        // reset squareLFO
        lfo.Reset();
        // do not reset random 

        int cntr = 0;
        while (cntr < loopLength)
        {
            // read audio in
            uint8_t audioValue = analogRead(audioPinIn) >> 2; // [0,256[

            devBoard.gateButtonIn.Update();//??
            
            //  write delay line
            delayLine.Write(writeHead.Get(), audioValue);

            // write to dac out for timing purposes (?)
            devBoard.dac.Write(0x7F);
            devBoard.dac.WriteR(0x7F);

            // advance delay line
            writeHead.Advance();

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
        writeHead.Reset(0);
        readHeadL.Reset(resetOffsetL);
        readHeadR.Reset(resetOffsetR);//TODO
        // reset squareLFO
        lfo.Reset();
        // do not reset random 

        uint8_t audioValue = 0x00;
        int cntr = 0;
        while (cntr < loopLength)
        {
            // read audio in
            audioValue += 9 + cntr%10 + cntr/200; // [0,256[

            devBoard.gateButtonIn.Update();//??
            
            //  write delay line
            delayLine.Write(writeHead.Get(), audioValue);

            // write to dac out for timing purposes (?)
            devBoard.dac.Write(0x7F);
            devBoard.dac.WriteR(0x7F);

            // advance delay line
            writeHead.Advance();

            // delay needed??
            // delayMicroseconds(delayusec);

            ++cntr;
        }

#ifdef DOSERIALDEBUG
        Serial.println('f');
#endif
    }

};
