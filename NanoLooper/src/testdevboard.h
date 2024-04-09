#pragma once
#include <Arduino.h>

#include "nanolooperapp.h"
#include "fastdigitaloutbank.h"
#include "shiftiobank.h"

void TestGateOut(DevBoard& board, int repeats)
{
    Serial.print("Test Gate out...");
    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        const int delayMSecs = 300;    
        for(int idx = 0;idx<2; ++idx)
        {
            board.SetGateOut(idx, 1);
            board.gateOut.Update();
            delay(delayMSecs);
            board.SetGateOut(idx, 0);
            board.gateOut.Update();
        }
    }
    Serial.println(" Done");
}

void TestLedOut(DevBoard& board, int repeats)
{
    Serial.print("Test Led out...");
    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        const int delayMSecs = 300;    
        for(int idx = 0;idx<8; ++idx)
        {
            board.SetLedOut(idx, 1);
            board.shiftIO.Update();
            delay(delayMSecs);
            board.SetLedOut(idx, 0);
            board.shiftIO.Update();
        }
    }
    Serial.println(" Done");
}

void TestPatchOut(DevBoard& board, int repeats)
{
    Serial.print("Test Patch out...");
    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        const int delayMSecs = 300;    
        for(int idx = 0;idx<8; ++idx)
        {
            board.SetPatchOut(idx, 1);
            board.shiftIO.Update();
            delay(delayMSecs);
            board.SetPatchOut(idx, 0);
            board.shiftIO.Update();
        }
    }
    Serial.println(" Done");
}

void TestGateOutToGateInToLedOut(DevBoard& board, int repeats)
{
    // connect gate out to gate in
    // alternating gate out high/low
    // leds reflect gate in state
    Serial.println("Test gate out to gate in to led ...");
    const int delayMSecs = 300;
    int msecs = 4*delayMSecs;
    for(int repeat = 0; repeat<msecs; ++repeat)
    {        
        board.SetGateOut(0, repeat<delayMSecs);
        board.SetGateOut(1, 2*delayMSecs<=repeat && repeat<3*delayMSecs);
        board.gateOut.Update();

        board.gateButtonIn.Update();

        board.SetLedOut(0, board.GetGateIn(0));
        board.SetLedOut(1, board.GetGateIn(1));
        board.shiftIO.Update();

        for(int idx = 0 ; idx<2; ++idx)
        {
            if(board.GetGateInRising(idx))
            {
                Serial.print("Gate ");
                Serial.print(idx);
                Serial.println(" is rising");
            }
            else if(board.GetGateInFalling(idx))
            {
                Serial.print("Gate ");
                Serial.print(idx);
                Serial.println(" is falling");
            }
        }


        delay(1);//realistic??
    }
    Serial.println(" Done");
}

void PatchInToLed(DevBoard& board)
{
    for(int idx = 0; idx<8; ++idx)
    {
        board.SetLedOut(idx, board.GetPatchIn(idx));
    }
}

void TestPatchOutToPatchInToLedOut(DevBoard& board)
{
    Serial.print("Test Patch out to patch in ...");
    // patch out set alternating gates out
    // led out based on gate in state
    // upon each change in gates out check changes in patch in
    const int delayMSecs = 300;
    for(int step = 0; step<8; ++ step)
    {
        board.SetPatchOut(step, 1);
        board.shiftIO.Update();
        PrintChanges(board.shiftIO);
        PatchInToLed(board);
        board.shiftIO.Update();
        delay(delayMSecs);

        board.SetPatchOut(step, 0);
        board.shiftIO.Update();
        PrintChanges(board.shiftIO);
        PatchInToLed(board);
        board.shiftIO.Update();
        delay(delayMSecs);
    }
    Serial.println(" Done");
}

void TestPatchOutToLedToPatchIn(DevBoard& board, int repeats)
{
    Serial.println("Test Patch out to patch in ...");
    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        for(int gate = 0; gate<8; ++gate)
        {
            //SetAll(board.shiftIO);
            ClearAll(board.shiftIO);
            board.SetPatchOut(gate, 1);
            board.SetLedOut(gate, 1);
            board.shiftIO.Update();//set out 
            // 2nd update to read in after out has been set
            board.shiftIO.Update();
            
            Serial.print(gate);
            Serial.print(' ');
            for(int ptch = 0; ptch<8; ++ptch)
            {
                Serial.print(board.GetPatchIn(ptch));
            }
            Serial.println();
            
            delay(300);
        }      
    }
     Serial.println(" Done");
}

void TestGateOutToAudioIn(DevBoard& board, int repeats)
{
    Serial.println("Test Gate out to audio in ...");
    // gate out set alternating gates out
    // led out based on gate out state
    // upon each change in gates out check changes in analog in
    const int delayMSecs = 1000;

    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        board.SetGateOut(0, 1);
        board.SetGateOut(1, 1);
        board.SetLedOut(0, 1);
        board.SetLedOut(1, 0);    
        board.gateOut.Update();// gate out
        board.shiftIO.Update();// leds
        delay(1);
        board.audioIn.Update();// read audio in
        PrintValues(board.audioIn);
        Serial.println();
        delay(delayMSecs);

        // board.SetGateOut(0, 0);
        // board.SetGateOut(1, 1);
        // board.SetLedOut(0, 0);
        // board.SetLedOut(1, 1);    
        // board.gateOut.Update();// gate out
        // board.shiftIO.Update();// leds
        // delay(1);
        // board.audioIn.Update();// read audio in
        // PrintValues(board.audioIn);
        // Serial.println();
        // delay(delayMSecs);
    }
    Serial.println(" Done");
}

void TestButtons(DevBoard& board, int repeats)
{
    Serial.println("Test Button in...");
    for(int idx = 0; idx<3000*repeats; ++ idx)
    {
        board.gateButtonIn.Update();
        for(int btn = 0; btn<4; ++btn)
        {
            if(board.GetButtonPressed(btn))
            {
                Serial.print("Button ");
                Serial.print(btn);
                Serial.println(" pressed");
            }
            // leds reflect button state
            board.SetLedOut(btn, board.GetButton(btn));
        }
        board.shiftIO.Update();// update leds
        delay(1);// is this realistic????
    }
    Serial.println(" Done");
}

void TestPots(DevBoard& board, int repeats)
{
    Serial.println("Test Pots...");
    for(int repeat = 0;repeat<repeats;++repeat)
    {
        board.potIn.UpdateAll();

        for(int idx = 0; idx<8; ++ idx)
        {
            Serial.print(idx);
            Serial.print(' ');
            Serial.println(board.GetPot(idx));
        }

        delay(500);        
    }
    Serial.println(" Done");
}

void TestPotsChanged(DevBoard& board, int repeats)
{
    Serial.println("Test Pots changed...");
    int msecs = 1000 * repeats;
    for(int repeat = 0; repeat<msecs; ++repeat)
    {
        board.potIn.Update();

        for(int idx = 0; idx<8; ++idx)
        {
            if(board.GetPotChanged(idx))
            {
                Serial.print("pot ");
                Serial.print(idx);
                Serial.print(" is changed to ");
                Serial.println(board.GetPot(idx));
            }
        }

        delay(1);//realistic??   
    }
    Serial.println(" Done");
}

