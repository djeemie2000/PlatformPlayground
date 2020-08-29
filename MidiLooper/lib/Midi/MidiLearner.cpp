#include "MidiLearner.h"

MidiLearner::MidiLearner(uint8_t midiChannel, uint8_t midiNote)
    : m_MidiChannel(midiChannel), m_LearnChannel(false), m_MidiNote(0x48), m_LearnNote(false)
{
    for (int idx = 0; idx < NumCC; ++idx)
    {
        m_MidiCC[idx] = 0x10 + idx; //????
        m_LearnCC[idx] = false;
    }
}
