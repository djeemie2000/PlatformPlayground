#include "OctaveMidiHandler.h"

uint8_t Transpose(uint8_t MidiNote, int transp)
{
    int note  = MidiNote + transp;
    if(note<0)
    {
        note= 0;
    }
    else if(127<note)
    {
        note = 127;
    }
    return note;
}
