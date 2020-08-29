#include "MidiMetronome.h"

MidiMetronome::MidiMetronome()
    : m_MidiLearn(false), m_MidiChannel(0x03),
      m_BaseNote(0x48), m_Divider(4), m_Divider2(4), m_LastChannel(0x03), m_LastNote(0xFF)
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

void MidiMetronome::OnTick(MidiLooperTicker &ticker, MidiOut &midiOut)
{
    // make sure note off is always sent, even after chaning midi channel upon learn
    const uint8_t velocity = 0x7F;
    if (ticker.clockIsFalling() && m_LastNote != 0xFF)
    {
        midiOut.NoteOff(m_LastChannel, m_LastNote, velocity);
        m_LastNote = 0xFF;
    }

    if (m_Playing && ticker.clockIsRising())
    {
        int step = ticker.recordingStep(1);
        if (step % m_Divider == 0)
        {
            m_LastNote = m_BaseNote;
            m_LastChannel = m_MidiChannel;
            if (step == 0)
            {
                m_LastNote += 24; // + 2 octaves
            }
            else if (step % (m_Divider2 * m_Divider) == 0)
            {
                m_LastNote += 12; //+1 octave
            }

            midiOut.NoteOn(m_LastChannel, m_LastNote, velocity);
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
