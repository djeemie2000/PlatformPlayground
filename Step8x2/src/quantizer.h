#pragma once
#include <Arduino.h>

class Quantizer
{
public:
    Quantizer();
    uint16_t analogInToMidiNote(uint16_t analogInValue)
    {
        // analogIn 1024 corresponds to 5V which corresponds to midi note 60 = 5*12 ( 1V/oct )
        // hence
        uint16_t midiNote = (analogInValue * 60u) >> 10;
        // note 60 * 1024 < 2^16 i.e. is in range for uint16_t
        return midiNote;
    }
    uint16_t cropAnalogOut(uint16_t analogOut)
    {
        return (analogOut<=4096) ? analogOut : 4095;
    }
    uint16_t midiNoteToAnalogOut(uint16_t midiNote)
    {
        // 1V/oct, 1V = 12 midi notes = 1000mV
        // analogOut 4096 = 4096 mV
        // need to crop at 4095!
        // hence
        uint16_t analogOut = midiNote * 1000u / 12u;
        // note 60 * 1024 < 2^16 i.e. is in range for uint16_t,
        // higher midi note may go out of range!

       return cropAnalogOut(analogOut);
    }
    uint16_t quantize(uint16_t analogInValue);
};
