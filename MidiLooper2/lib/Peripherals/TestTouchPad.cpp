#include "TestTouchPad.h"

void PrintTouchPad(MPR121TouchPad &touchPad, HardwareSerial &debugSerial)
{
    debugSerial.print("touchPad ");
    for (int pad = 0; pad < touchPad.GetNumPads(); ++pad)
    {
        if (touchPad.Get(pad))
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

void TestTouchPad(MPR121TouchPad &touchPad, HardwareSerial &debugSerial, int count)
{
    int cntr = 0;
    while (cntr < count || count == -1)
    {
        touchPad.Read();
        PrintTouchPad(touchPad, debugSerial);
        ++cntr;
        delay(500);
    }
}
