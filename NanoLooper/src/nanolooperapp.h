#pragma once
#include <Arduino.h>

#include "delayline.h"
#include "fastdac.h"
#include "fastdigitalwrite.h"
#include "lowpassfilter.h"
#include "pseudorandom.h"
#include "bitcrush.h"
#include "analoginbank.h"

struct NanoLooperApp
{
    static const int loopLength = 1024 + 512 + 128; // 1.625 kB
    // output pins
    static const int squareLFOOutPin = LED_BUILTIN; // 13 PB5
    static const int randomOutPin = 12;             // PB4
    // button/gate in pins
    static const int recordingButtonInPin = 2; // PD2
    static const int degradeInPin = 3;         // PD3
    static const int saturateInPin = A3;       // PC3
    static const int reverseInPin = A4;        // PC4
    static const int resetInPin = A5;          // PC5
    // analog in pins
    static const int audioPinIn = A1; // PC1

    static const int speedInPin = A0;           // PC0
    static const int squareLFOPeriodInPin = A2; // PC2
    static const int bitcrushInPin = A6;
    static const int randomProbabilityInPin = A7;

    DelayLine<loopLength> delayLine; // assumes 328 2k memory
    FastDac dac;                     // assumes arduino nano
    AnalogInBank<4> analogIn;

    // random out (1/0)
    PseudoRandom pseudoRandom;
    int randomState;
    uint16_t randomThreshold;

    int prevRecordingBtn;

    int prevResetGate;

    int delayusec;

    // square LFO out (1/0)
    int squareLFOCounter;
    int squareLFOPeriod;
    int squareLFOState;

    // bitcrush
    int bitcrush;

    NanoLooperApp()
    {
    }

    void Begin()
    {
        delayLine.Begin();
        dac.Begin();
        // button/gate in
        pinMode(recordingButtonInPin, INPUT_PULLUP);
        pinMode(degradeInPin, INPUT_PULLUP);
        pinMode(saturateInPin, INPUT_PULLUP);
        pinMode(reverseInPin, INPUT_PULLUP);
        pinMode(resetInPin, INPUT_PULLUP);
        // outputs
        pinMode(squareLFOOutPin, OUTPUT);
        pinMode(randomOutPin, OUTPUT);
        // analogIn
        analogIn.Begin();
        analogIn.Assign(0, speedInPin);
        analogIn.Assign(1, squareLFOPeriodInPin);
        analogIn.Assign(2, randomProbabilityInPin);
        analogIn.Assign(3, bitcrushInPin);

        prevRecordingBtn = 0;
        prevResetGate = 0;
        delayusec = 0;
        squareLFOCounter = 0;
        squareLFOPeriod = 16000;
        randomState = 1;
        randomThreshold = 128;
        bitcrush = 0;
        squareLFOState = 1;

        analogIn.UpdateAll();
    }

    bool RecordingBtnClicked()
    {
        int recordingBtn = fastDigitalReadPortD<2>(); // PD2
        bool clicked = !prevRecordingBtn && recordingBtn;
        prevRecordingBtn = recordingBtn;
        return clicked;
    }

    bool ResetGateRising()
    {
        // A5 //PC5
        int gate = fastDigitalReadPortC<5>();//PC5
        bool rising = !prevResetGate && gate;
        prevResetGate = gate;
        return rising;
    }

    void update()
    {
        // first read record button in
        if (RecordingBtnClicked())
        {
            DoRecording();
        }
        else
        {
            DoPlayback();
        }
    }

    void DoPlayback()
    {
        // first read CV(s)
        // alternate between CV ins 
        analogIn.Update();

        delayusec = analogIn.Get(0) >> 1;   // [0-512[ microseconds        
        squareLFOPeriod = analogIn.Get(1) << 3; // [0-32768/4[
        squareLFOPeriod = max(32, squareLFOPeriod);        
        randomThreshold = analogIn.Get(2); // [0, 1024[ ??
        bitcrush = analogIn.Get(3) >> 6; //[0,15] ?? more types?

        // update square LFO
        if (squareLFOPeriod < squareLFOCounter)
        {            
            squareLFOCounter = 0;
            squareLFOState = 1 - squareLFOState;
#ifdef DOSERIALDEBUG
            Serial.println(resetPeriod);
#endif
        }
        fastDigitalWritePortB<5>(squareLFOState); // pin13 PB5
        ++squareLFOCounter;

        // update random out
        uint32_t random = pseudoRandom.Rand();
        // compare with threshold ~ CV/pot
        if ((random & 0xFFFF) < randomThreshold)
        {
            // toggle state
            randomState = 1 - randomState;
        }
        fastDigitalWritePortB<4>(randomState);

        if(ResetGateRising())
        {
            delayLine.Reset();
        }

        // degrade/saturate sample
        if (!fastDigitalReadPortD<3>())
        {
            delayLine.Degrade();
        }
        if (!fastDigitalReadPortC<3>())
        {
            delayLine.Saturate();
        }

        // read audio from delay line
        uint8_t audioOut = delayLine.Read();

        // process/bitcrush audio 
        // ~ CV [0, 15]
        if(bitcrush<4)
        {
            // 0 .. 3
            audioOut = BitCrush3(audioOut, 2*bitcrush);// 0 2 4 6
        }
        else if(bitcrush<8)
        {
            //4..7
             audioOut = BitCrush1(audioOut, 1+2*(bitcrush-4)); // 1 3 5 7
        }
        else if(bitcrush<12)
        {
            //8..11
             audioOut = BitCrush1b(audioOut, 1+2*(bitcrush-8)); // 1 3 5 7
        }
        else
        {
            //12..15
            audioOut = BitCrush2(audioOut, bitcrush - 11);
        }        

        // write audio to fast dac out
        dac.Write(audioOut);

        // advance/reverse delay line
        if (fastDigitalReadPortC<4>())
        {
            delayLine.Advance();
        }
        else
        {
            delayLine.Reverse();
        }

        // delay ~ speed cv
        delayMicroseconds(delayusec);
    }

    void DoRecording()
    {
#ifdef DOSERIALDEBUG
        Serial.println('R');
#endif

        // always sample at highest freq

        // reset delay line
        delayLine.Reset();
        squareLFOCounter = 0;

        int cntr = 0;
        while (cntr < loopLength)
        {
            // read audio in
            uint8_t audioValue = analogRead(audioPinIn) >> 2; // [0,256[

            // recording led on
            fastDigitalWritePortB<5>(1); // pin13 PB5

            //  write delay line
            delayLine.Write(audioValue);

            // read delay line
            //    write to fast dac out
            dac.Write(0x7F); // delayLine.Read());

            // advance delay line
            delayLine.Advance();

            // delay ~ speed cv
            // delayMicroseconds(delayusec);

            ++cntr;
        }

#ifdef DOSERIALDEBUG
        Serial.println('r');
#endif
    }
};
