#include "MidiMetronome.h"

MidiMetronome::MidiMetronome()
    : m_MidiLearn(false), m_MidiChannel(0x03),
      m_BaseNote(0x48), m_TicksPerBeat(4), m_BeatsPerBar(4), m_LastChannel(0x03), m_LastNote(0xFF)
{
}

void MidiMetronome::Start()
{
    m_Playing = true;
}
void MidiMetronome::Stop()
{
    m_Playing = false;
    // note off in OnTick
}

void MidiMetronome::NoteOn(MidiOut &midiOut, uint8_t channel, uint8_t midiNote)
{
    m_LastChannel = channel;
    m_LastNote = midiNote;
    const uint8_t velocity = 0x7F;
    midiOut.NoteOn(m_LastChannel, m_LastNote, velocity);
}

void MidiMetronome::NoteOff(MidiOut &midiOut)
{
    // make sure note off is always sent, even after changing midi channel upon learn
    if(m_LastNote != 0xFF)
    {
        const uint8_t velocity = 0x7F;
        midiOut.NoteOff(m_LastChannel, m_LastNote, velocity);
        m_LastNote = 0xFF;
    }
}


void MidiMetronome::OnTick(MidiLooperTicker &ticker, MidiOut &midiOut)
{
    // make sure note off is always sent, even after changing midi channel upon learn
    const uint8_t velocity = 0x7F;
    if (ticker.clockIsFalling())
    {
        NoteOff(midiOut);
    }

    if (m_Playing && ticker.clockIsRising())
    {
        const int ClocksPerTick = 1;
        uint16_t counter = ticker.Counter(ClocksPerTick);//Counter(1) ???
        uint16_t tick = CounterToTick(counter);
        if (tick == 0)
        {
            // first tick of beat
            uint8_t midiNote = m_BaseNote;

            uint16_t beat = CounterToBeat(counter);
            if (beat == 0)
            {
                // first beat of a bar
                midiNote += 12; //+1 octave
                
                uint16_t bar = CounterToBar(counter);
                if(bar == 0)
                {
                    // first beat of first bar
                    midiNote += 12; // + another 1 octave
                }
            }

            NoteOn(midiOut, m_MidiChannel, midiNote);
        }
    }
}

void MidiMetronome::StartMidiLearn()
{
    m_MidiLearn = true;
}

void MidiMetronome::OnNoteOn(uint8_t channel, uint8_t midiNote, uint8_t velocity)
{
    //TODO learn channel + base note
    if (m_MidiLearn)
    {
        m_MidiChannel = channel;
        m_BaseNote = midiNote;
        m_MidiLearn = false;
    }
}
