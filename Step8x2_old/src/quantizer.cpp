#include "quantizer.h"

 Quantizer::Quantizer()
 {}

uint16_t Quantizer::quantize(uint16_t analogInValue)
{
    return midiNoteToAnalogOut(analogInToMidiNote(analogInValue));
}

 