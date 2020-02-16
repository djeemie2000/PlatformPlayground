#include "TestTouchPad.h"

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

void TestTouchPad(TTP8229TouchPad& touchPad, SerialOut& debugSerial, int count)
{
    int cntr = 0;
    while(cntr<count ||count==-1)
    {
        touchPad.Read();
        PrintTouchPad(touchPad, debugSerial);
        delay(500);
    }
}

