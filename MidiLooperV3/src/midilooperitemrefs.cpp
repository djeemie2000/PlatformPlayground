#include "midilooperitemrefs.h"

MidiLooperItemRefs::MidiLooperItemRefs() 
: size(0)
{}

void MidiLooperItemRefs::AddReference(uint8_t idx)
{
    refs[size] = idx;
    ++size;
}

void MidiLooperItemRefs::RemoveReference()
{
    if(0<size)
    {
        --size;
    }
}
