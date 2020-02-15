#pragma once

#include <Arduino.h>
#include "../IO/SerialOut.h"
#include "TTP8229TouchPad.h"

void PrintTouchPad(TTP8229TouchPad& touchPad, SerialOut& debugSerial)
{
    debugSerial.print("touchPad ");
    for(int pad = 0; pad<touchPad.GetNumPads(); ++pad)
    {
        if(touchPad.Get(pad))
        {
            debugSerial.print("1");
        }
        else
        {
            debugSerial.print("0");
        }
    }
    debugSerial.println();
}

void TestTouchPad(TTP8229TouchPad& touchPad, SerialOut& debugSerial)
{
    while(true)
    {
        PrintTouchPad(touchPad, debugSerial);
        delay(300);
    }
}
