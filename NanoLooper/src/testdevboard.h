#pragma once
#include <Arduino.h>

#include "nanolooperapp.h"
#include "fastdigitaloutbank.h"
#include "shiftiobank.h"
//TODO DevBoard vs App

void TestGateOut(DevBoard& board)
{
    TestDigitalOutBank(board.gateOut, 5);
}

void TestLedOut(DevBoard& board)
{
    TestDigitalOutBank(board.shiftIO, 5);
}