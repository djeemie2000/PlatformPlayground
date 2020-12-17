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
    if (ticker.clockIsFalling())
    {
        NoteOff(midiOut);
    }

    if (m_Playing && ticker.clockIsRising())
    {
        TickerState state;
        ticker.GetTickerState(state);
        if (state.m_Tick == 0)
        {
            // first tick of beat
            uint8_t midiNote = m_BaseNote;

            if (state.m_Beat == 0)
            {
                // first beat of a bar
                midiNote += 12; //+1 octave
                
                if(state.m_Bar == 0)
                {
                    // first beat of first bar
                    midiNote += 12; // + another 1 octave
                }
            }

            NoteOn(midiOut, m_MidiChannel, midiNote);
        }
    }
}

void MidiMetronome::ToggleMidiLearn()
{
    m_MidiLearn = !m_MidiLearn;
}

void MidiMetronome::onToggleMuted()
{
    m_Playing = !m_Playing;
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

void MidiMetronome::printState(HardwareSerial& serial)
{
    serial.print("Ch");
    serial.print(m_MidiChannel, HEX);
    serial.print(" L");
    serial.print(m_MidiLearn ? 1 : 0);
    serial.print("    P");
    serial.println(m_Playing ? 1 : 0);
}
