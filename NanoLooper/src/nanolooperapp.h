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
#include "nanolooperspeedtest.h"

#define DOSERIALDEBUG 1

struct NanoLooperApp
{
    static const int loopLength = 1024 + 512;// + 128; // 1.625 kB = 7/4 kB

    DevBoard devBoard;//TODO 

    DelayLine<loopLength> delayLine; // assumes atmega328 2k memory
    TapeHead<loopLength> writeHead;
    TapeHead<loopLength> readHeadL;
    TapeHead<loopLength> readHeadR;

    // square LFO out (1/0)
    SquareLFOState<4> lfo;
    // random out (1/0)
    PseudoRandomState<4> pseudoRandom;

    static const int loopingMode = 1;
    static const int processingMode = 1;
    int mode;

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

        lfo.Begin();
        lfo.SetPeriod(0, 8000);
        lfo.SetPeriod(1, 4000);
        lfo.SetPeriod(2, 8000);
        lfo.SetPeriod(3, 4000);
        
        pseudoRandom.Begin();
        pseudoRandom.SetThreshold(0, 128);
        pseudoRandom.SetThreshold(1, 128);
        pseudoRandom.SetThreshold(2, 128);
        pseudoRandom.SetThreshold(3, 128);

        devBoard.potIn.UpdateAll();

        setupFastAnalogRead(2);

        mode = processingMode;
    }

    void update()
    {
        devBoard.gateButtonIn.Update();
        if (devBoard.GetButtonPressed(0))
        {
            // record
            DoRecording();
            // then loop the recording
            mode = loopingMode;
        }
        else if(devBoard.GetButtonPressed(1))
        {
            // process audio in
            mode = processingMode;
        }
        else if(devBoard.GetButtonPressed(2))
        {
            // loop the recording
            mode = loopingMode;
        }

        if(mode == processingMode)
        {
            DoProcessing();
        }
        else
        {
            DoLooping();
        }
    }

    void UpdatePatchIO()
    {
        // pot 2 6 -> LFO and random controls L R
        // update square LFO
        int squareLFOPeriod1 = devBoard.GetPot(2) << 3; // [0-32768/4[
        squareLFOPeriod1 = max(32, squareLFOPeriod1);        
        int squareLFOPeriod2 = devBoard.GetPot(6) << 3; // [0-32768/4[
        squareLFOPeriod2 = max(32, squareLFOPeriod2);
        lfo.SetPeriod(0, squareLFOPeriod1);
        lfo.SetPeriod(1, squareLFOPeriod1/2);   
        lfo.SetPeriod(2, squareLFOPeriod2);
        lfo.SetPeriod(3, squareLFOPeriod2/2);   
        lfo.Update();

        // update random out pot 4 5
        pseudoRandom.SetThreshold(0, devBoard.GetPot(2));
        pseudoRandom.SetThreshold(1, devBoard.GetPot(2)/2);
        pseudoRandom.SetThreshold(2, devBoard.GetPot(6));
        pseudoRandom.SetThreshold(3, devBoard.GetPot(6)/2);
        pseudoRandom.Update();

        // set patch out 
        // led out -> hack idx for hardware issue
        devBoard.SetPatchOut(0, lfo.Get(0));
        devBoard.SetLedOut(0, lfo.Get(0));
        devBoard.SetPatchOut(1, lfo.Get(1));
        devBoard.SetLedOut(1, lfo.Get(1));
        devBoard.SetPatchOut(2, pseudoRandom.Get(0));
        devBoard.SetLedOut(2, pseudoRandom.Get(0));
        devBoard.SetPatchOut(3, pseudoRandom.Get(1));
        devBoard.SetLedOut(7, pseudoRandom.Get(1));
        devBoard.SetPatchOut(4, lfo.Get(2));
        devBoard.SetLedOut(3, lfo.Get(2));
        devBoard.SetPatchOut(5, lfo.Get(3));
        devBoard.SetLedOut(4, lfo.Get(3));
        devBoard.SetPatchOut(6, pseudoRandom.Get(2));
        devBoard.SetLedOut(5, pseudoRandom.Get(2));
        devBoard.SetPatchOut(7, pseudoRandom.Get(3));
        devBoard.SetLedOut(6, pseudoRandom.Get(3));

    }

    int CalcMixedAudio(uint8_t newByte, uint8_t oldByte, int feedback, int feedbackoffset)
    {
        int newSigned = 128 - newByte;// invert????//newByte - 128;
        int oldSigned = oldByte - 128;
        int mixedSigned = (feedbackoffset*oldSigned + feedback*oldSigned + (256-feedback)*newSigned)>>8;
        return 128+mixedSigned;
    }

    void DoProcessing()
    {
        // first read CV(s),gate in, patch in
        devBoard.potIn.Update();
        devBoard.gateButtonIn.Update();
        devBoard.shiftIO.Update();

        UpdatePatchIO();

        // TODO delay needed???
        // pot 0 speed
        int delayusec = devBoard.GetPot(0) >> 2;   // [0-512[ microseconds        

        // pot 4 feedback
        int feedback = devBoard.GetPot(4)>>2;//  [0, 256]

        // optimize read only 1 audio in (left)
        devBoard.audioIn.Update(0);

        // int newAudio = devBoard.GetAudioInL8bit() - 128;
        // int oldAudio = delayLine.Read(writeHead.Get()) - 128;
        // int mixedAudio = (feedback*oldAudio + (256-feedback)*newAudio)>>8;
        // delayLine.Write(writeHead.Get(), mixedAudio+128);

        delayLine.Write(writeHead.Get(), 
                        CalcMixedAudio(devBoard.GetAudioInL8bit(), delayLine.Read(writeHead.Get()), feedback, 32));
        //always (!) advance write head !!!
        writeHead.Advance();


        // reset position pots 1 5
        int resetOffsetL = devBoard.GetPot(1)*3/2; // [0,1024+512[  
        int resetOffsetR = devBoard.GetPot(5)*3/2; // [0,1024+512[  
        
        // reset upon patch 0 4 rising
        if(devBoard.GetPatchInRising(0))
        {
            // reset position is read from analogIn
            // // backwards wrt write head!
            // readHeadL.Reset(writeHead.Get());
            readHeadL.Reverse(resetOffsetL);
#ifdef DOSERIALDEBUG
            Serial.println(resetOffsetL);
#endif
        }

        if(devBoard.GetPatchInRising(4))
        {
            // readHeadR.Reset(writeHead.Get());
            readHeadR.Reverse(resetOffsetR);
            // backwards wrt write head!
#ifdef DOSERIALDEBUG
            Serial.println(resetOffsetR);
#endif
        }

        // read audio from delay line
        // degrade pot 3 saturate pot 7
        int degrade = devBoard.GetPot(3) >> 3;// [0,128[
        int saturate = devBoard.GetPot(7) >> 3;// [0,128[

        uint8_t audioOutL = delayLine.Read(readHeadL.Get());    
        audioOutL = Degrade(audioOutL, degrade);
        audioOutL = Saturate(audioOutL, saturate);

        uint8_t audioOutR = delayLine.Read(readHeadR.Get());    
        audioOutR = Degrade(audioOutR, degrade);
        audioOutR = Saturate(audioOutR, saturate);

        // write audio to fast dac out
        // flipped patch in 2 6
        if(devBoard.GetPatchIn(2))
        {
            devBoard.dac.WriteFlipped(audioOutL);
        }
        else
        {
            devBoard.dac.Write(audioOutL);
        }

        if(devBoard.GetPatchIn(6))
        {
            devBoard.dac.WriteRFlipped(audioOutR);
        }
        else
        {
            devBoard.dac.WriteR(audioOutR);
        }

        // advance/reverse delay line
        if (devBoard.GetPatchIn(1))
        {
            readHeadL.Reverse();
        }
        else
        {
            readHeadL.Advance();
        }

        if(devBoard.GetPatchIn(5))
        {
            readHeadR.Reverse();
        }
        else
        {
            readHeadR.Advance();
        }

        // gate out
        devBoard.gateOut.Update();

        // delay ~ speed cv
        if(devBoard.GetPatchIn(7))
        {
            delayMicroseconds(2*delayusec);
        }
        else
        {
            delayMicroseconds(delayusec);
        }

    }

    void DoLooping()
    {
        // first read CV(s),gate in, patch in
        devBoard.potIn.Update();
        devBoard.gateButtonIn.Update();
        devBoard.shiftIO.Update();

        UpdatePatchIO();

        // pot 0 speed
        int delayusec = devBoard.GetPot(0) >> 1;   // [0-512[ microseconds        
        // TODO mod delay based on audio in R * mod depth pot 4
        
        // reset position pots 1 5
        int resetOffsetL = devBoard.GetPot(1)*3/2; // [0,1024+512[  
        int resetOffsetR = devBoard.GetPot(5)*3/2; // [0,1024+512[  
        
        // reset upon patch 0 4 rising
        if(devBoard.GetPatchInRising(0))
        {
            // reset position is read from analogIn
            readHeadL.Reset(resetOffsetL);
#ifdef DOSERIALDEBUG
            Serial.println(resetOffsetL);
#endif
        }

        if(devBoard.GetPatchInRising(4))
        {
            readHeadR.Reset(resetOffsetR);
#ifdef DOSERIALDEBUG
            Serial.println(resetOffsetR);
#endif
        }

        // read audio from delay line
        // degrade pot 3 saturate pot 7
        int degrade = devBoard.GetPot(3) >> 3;// [0,128[
        int saturate = devBoard.GetPot(7) >> 3;// [0,128[

        uint8_t audioOutL = delayLine.Read(readHeadL.Get());    
        audioOutL = Degrade(audioOutL, degrade);
        audioOutL = Saturate(audioOutL, saturate);

        uint8_t audioOutR = delayLine.Read(readHeadR.Get());    
        audioOutR = Degrade(audioOutR, degrade);
        audioOutR = Saturate(audioOutR, saturate);

        // write audio to fast dac out
        // flipped patch in 2 6
        if(devBoard.GetPatchIn(2))
        {
            devBoard.dac.WriteFlipped(audioOutL);
        }
        else
        {
            devBoard.dac.Write(audioOutL);
        }

        if(devBoard.GetPatchIn(6))
        {
            devBoard.dac.WriteRFlipped(audioOutR);
        }
        else
        {
            devBoard.dac.WriteR(audioOutR);
        }

        // advance/reverse delay line
        if (devBoard.GetPatchIn(1))
        {
            readHeadL.Reverse();
        }
        else
        {
            readHeadL.Advance();
        }

        if(devBoard.GetPatchIn(5))
        {
            readHeadR.Reverse();
        }
        else
        {
            readHeadR.Advance();
        }

        // gate out
        devBoard.gateOut.Update();

        // patch out => next update
        // devBoard.shiftIO.Update();

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
        readHeadL.Reset(0);
        readHeadR.Reset(0);
        // reset squareLFO
        lfo.Reset();
        // do not reset random 

        int cntr = 0;
        while (cntr < loopLength)
        {
            // read audio in
            //TODO only single audio in (L) update!!!!
            devBoard.audioIn.UpdateAll();
            
            uint8_t audioValue = devBoard.GetAudioInL8bit();//analogRead(audioPinIn) >> 2; // [0,256[

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
        readHeadL.Reset(0);
        readHeadR.Reset(0);
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
