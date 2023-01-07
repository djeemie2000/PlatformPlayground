#include "midiloopermetronome.h"
#include "midiout.h"

MidiLooperMetronome::MidiLooperMetronome()
    : m_Channel(0xFF), m_BaseNote(0xFF), m_MidiNoteMask(), m_PlayMute(false), m_LastChannel(0x0F), m_LastNote(0xFF)
{
}

bool MidiLooperMetronome::AllNotesOff(HardwareSerial &serial)
{
    if (m_LastNote != 0xFF)
    {
        const uint8_t velocity = 0x7F;
        NoteOff(serial, m_MidiNoteMask, m_LastChannel, m_LastNote, velocity);
        m_LastNote = 0xFF;
    }

    return true;
}

bool MidiLooperMetronome::LearnNoteOn(uint8_t channel, uint8_t midiNote, HardwareSerial &serial)
{
    // all note off for active notes on current channel
    if (channel != m_Channel)
    {
        AllNotesOff(serial);
    }

    // update channel
    m_Channel = channel;
    m_BaseNote = midiNote;

    // return true if learned, false if not
    return true;
}

bool MidiLooperMetronome::PlayClockOn(int tick, int beat, int bar, HardwareSerial &serial)
{
    // play -> mute => all active notes off (once) -> cfr SetPlayMute()
    // when muted => do nothing
    if(!m_PlayMute)
    {
        return false;
    }

    if (tick == 0)
    {
        // first tick of every beat => note on
        uint8_t midiNote = m_BaseNote;
        if (beat == 0)
        {
            // first beat of a bar
            midiNote += 12; //+1 octave

            if (bar == 0)
            {
                // first beat of first bar
                midiNote += 12; // + another 1 octave
            }
        }

        const uint8_t velocity = 0x7F;
        NoteOn(serial, m_MidiNoteMask, m_Channel, midiNote, velocity);

        m_LastChannel = m_Channel;
        m_LastNote = midiNote;
    }

    return true;
}

bool MidiLooperMetronome::PlayClockOff(HardwareSerial &serial)
{
    // play -> mute => all active notes off (once) -> cfr SetPlayMute()
    // when muted => do nothing
    if(!m_PlayMute)
    {
        return false;
    }

    AllNotesOff(serial);
    return true;
}

bool MidiLooperMetronome::SetPlayMute(bool playMute, HardwareSerial &serial)
{
    if (m_PlayMute != playMute)
    {
        if (m_PlayMute)
        {
            AllNotesOff(serial);
        }
        m_PlayMute = playMute;
        return true;
    }
    return false;
}
