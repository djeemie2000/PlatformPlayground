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
#include "shiftiobank.h"
#include "tapehead.h"
#include "squarelfostate.h"
#include "pseudorandomstate.h"

#define DOSERIALDEBUG 1

//TODO devBoard vs app => reuseable devBoard

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

    DelayLine<loopLength> delayLine; // assumes 328 2k memory
    TapeHead<loopLength> writeHead;
    TapeHead<loopLength> readHeadL;
    TapeHead<loopLength> readHeadR;

    // MCP dac uses CS pin 10 
    MCPDac dac;                     // assumes arduino nano
    // 2x gate in pin D2 D3
    // 4x button in pin D6 D7 A4 A5 -> TODO debouncing
    FastDigitalInBank<6> gateButtonIn; // assumes arduino nano
    // 2x gate out pin D4 D5
    FastDigitalOutBank<2> gateOut; // assumes arduino nano
    // 8 to 3 analog in: Ain A3 + Din A0 A1 A2 = PORTC 0 1 2
    AnalogInBank821<0,1,2> analogIn; // assumes arduino nano
    // TODO shift register in x1 + out x2 uses CS pin D9
    ShiftIOBank<16> shiftIO;

    // random out (1/0)
    PseudoRandomState<2> pseudoRandom;

    int delayusec;
    int resetOffsetL;
    int resetOffsetR;

    uint8_t degrade;
    uint8_t saturate;

    // square LFO out (1/0)
    SquareLFOState<2> lfo;

    NanoLooperApp()
    {
    }

    void Begin()
    {
        delayLine.Begin();
        writeHead.Reset(0);
        readHeadL.Reset(0);
        readHeadR.Reset(0);

        dac.Begin();
        // gate in
        gateButtonIn.Assign(0, 2, true);
        gateButtonIn.Assign(1, 3, true);
        // button in
        gateButtonIn.Assign(2, 6, true);
        gateButtonIn.Assign(3, 7, true);
        gateButtonIn.Assign(4, A4, true);
        gateButtonIn.Assign(5, A5, true);
        // gate out
        gateOut.Assign(0, 4);
        gateOut.Assign(1, 5);
        // analog in 8 to 3
        analogIn.Begin(A3);
        // shift register IO
        shiftIO.Begin(9);

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

        analogIn.UpdateAll();
    }

    bool RecordingBtnClicked()
    {
        return gateButtonIn.IsFalling(0);
    }

    bool ResetGateRising()
    {
        return gateButtonIn.IsRising(2);
    }

    void update()
    {
        // first read record button in
        gateButtonIn.Update();
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
        resetOffsetL = analogIn.Get(1); // [0,1024[ TODO 
        resetOffsetR = analogIn.Get(2); // [0,1024[ TODO 
        
        degrade = analogIn.Get(6) >> 3;// [0,128[
        saturate = analogIn.Get(7) >> 3;// [0,128[

        // update square LFO
        int squareLFOPeriod = analogIn.Get(3) << 3; // [0-32768/4[
        squareLFOPeriod = max(32, squareLFOPeriod);        
        lfo.SetPeriod(0, squareLFOPeriod);
        lfo.SetPeriod(1, squareLFOPeriod/2);   
        lfo.Update();
        gateOut.Set(0, lfo.Get(0));
        gateOut.Set(2, lfo.Get(1));

        // update random out
        pseudoRandom.SetThreshold(0, analogIn.Get(4));// [0, 1024[ ??
        pseudoRandom.SetThreshold(1, analogIn.Get(4)/2);
        pseudoRandom.Update();
        gateOut.Set(1, pseudoRandom.Get(0));
        gateOut.Set(3, pseudoRandom.Get(1));

        // reset gate rising
        if(gateButtonIn.IsRising(2))
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
        if(gateButtonIn.Get(3))
        {
            dac.Write(audioOutL);
            dac.WriteR(audioOutR);
        }
        else
        {
            dac.WriteFlipped(audioOutL);
            dac.WriteRFlipped(audioOutR);
        }

        // advance/reverse delay line
        if (!gateButtonIn.Get(1))
        {
            readHeadL.Reverse();
            readHeadR.Reverse();
        }
        else
        {
            readHeadL.Advance();
            readHeadR.Advance();
        }

        gateOut.Update();

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

            gateButtonIn.Update();//??
            
            //  write delay line
            delayLine.Write(writeHead.Get(), audioValue);

            // write to dac out for timing purposes (?)
            dac.Write(0x7F);
            dac.WriteR(0x7F);

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

            gateButtonIn.Update();//??
            
            //  write delay line
            delayLine.Write(writeHead.Get(), audioValue);

            // write to dac out for timing purposes (?)
            dac.Write(0x7F);
            dac.WriteR(0x7F);

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
