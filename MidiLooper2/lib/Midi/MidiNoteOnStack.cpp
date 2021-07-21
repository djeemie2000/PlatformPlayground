#include "MidiNoteOnStack.h"

MidiNoteOnStack::MidiNoteOnStack()
{
    for(int idx = 0; idx<8; ++idx)
    {
        m_Bits[idx] = 0x0;
    }
}

uint16_t MidiNoteOnStack::GetNoteOn(uint8_t midiNote) const
{
    uint16_t idx = midiNote >> 4;
    uint16_t bit = midiNote & 0x0F;
    return bitRead(m_Bits[idx], bit);
}

void MidiNoteOnStack::SetNoteOn(uint8_t midiNote)
{
    uint16_t idx = midiNote >> 4;
    uint16_t bit = midiNote & 0x0F;
    bitSet(m_Bits[idx], bit);
}

void MidiNoteOnStack::SetNoteOff(uint8_t midiNote)
{
    uint16_t idx = midiNote >> 4;
    uint16_t bit = midiNote & 0x0F;
    bitClear(m_Bits[idx], bit);
}
