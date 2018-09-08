#include "ModeMidiHandler.h"


void ModeMidiHandler::PitchWheel(uint8_t Channel, int Bend)
{
    if(Channel == m_Channel)
    {
        m_midiOut.PitchWheel(Channel, Bend);
    }
}

