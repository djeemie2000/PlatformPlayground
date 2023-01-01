#include "midi2pgvg.h"
#include "ledout.h"
#include "gateoutbank.h"
#include "cvoutbank.h"
#include "EEPROM.h"

Midi2PGVG::Midi2PGVG()
    : m_LearnIndex(-1)
{
    for (int voice = 0; voice < NumVoices; ++voice)
    {
        m_Channel[voice] = 0x00;
        m_MidiNote[voice] = 0x00;
        m_MidiNote2[voice] = 0x00;
        m_IsActive[voice] = 0;
    }

    for(int ch = 0;ch<NumMidiChannels; ++ch)
    {
        m_ChannelCount[ch] = 0;
        m_ChannelBaseNote[ch] = 0;
    }
    m_ChannelCount[0x00] = NumVoices;
}

void Midi2PGVG::Begin(GateOutBank<Midi2PGVG::NumVoices*2> *gates, LedOut *ledOut, CVOutBank<Midi2PGVG::NumVoices*2> *cvOuts)
{
    m_Gates = gates;
    m_LedOut = ledOut;
    m_CvOuts = cvOuts;

    for (int voice = 0; voice < NumVoices; ++voice)
    {
        m_Channel[voice] = 0x00;
        m_MidiNote[voice] = 0x00;
        m_MidiNote2[voice] = 0x00;
        m_IsActive[voice] = 0;
        m_Gates->GateOff(voice * 2);
        m_Gates->GateOff(voice * 2 + 1);
        m_CvOuts->PitchOut(voice * 2, 0x00, 0x00);
        m_CvOuts->VelocityOut(voice * 2 + 1, 0x00);
    }

    for(int ch = 0; ch<NumMidiChannels; ++ch)
    {
        m_ChannelCount[ch] = 0;
        m_ChannelBaseNote[ch] = 0x00;
    }
    m_ChannelCount[0x00] = NumVoices;

    // status led on
    m_LedOut->LedOn();
}

void Midi2PGVG::VoiceOn(int voice, uint8_t midiBaseNote, uint8_t midiNote, uint8_t velocity)
{
    m_Gates->GateOn(voice * 2);
    m_Gates->GateOn(voice * 2 + 1);
    m_CvOuts->PitchOut(voice * 2, midiBaseNote, midiNote);
    m_CvOuts->VelocityOut(voice * 2 + 1, velocity);
}

void Midi2PGVG::VoiceOn(int voice, uint8_t midiBaseNote, uint8_t midiNote)
{
    // keep velocity
    m_Gates->GateOn(voice * 2);
    m_Gates->GateOn(voice * 2 + 1);
    m_CvOuts->PitchOut(voice * 2, midiBaseNote, midiNote);
}

void Midi2PGVG::VoiceOff(int voice)
{
    // keep midi note, keep velocity
    // update gate out
    m_Gates->GateOff(voice * 2);
    m_Gates->GateOff(voice * 2 + 1);
}


void Midi2PGVG::OnMessage(MidiVoiceMessage &message)
{
    if (IsLearning())
    {
        if (IsNoteOn(message))
        {
            uint8_t prevChannel = m_Channel[m_LearnIndex]; 
            --m_ChannelCount[prevChannel];

            uint8_t channel = Channel(message);
            m_Channel[m_LearnIndex] = channel;
            m_ChannelBaseNote[channel] = MidiNote(message);
            ++m_ChannelCount[channel];

            ++m_LearnIndex;
            if (NumVoices <= m_LearnIndex)
            {
                // all voices learned
                m_LearnIndex = -1;

                m_LedOut->LedOn();
            }
        }
    }
    else
    {
        // normal operation (poly mode)
        // first voice matching the channel that is not active yet will handle a note on
        // note off => handled by active voice when matching channel and note
        uint8_t channel = Channel(message);
        uint8_t midiNote = MidiNote(message);
        uint8_t velocity = Velocity(message);
        bool handled = false;
        for (int voice = 0; voice < NumVoices && !handled; ++voice)
        {
            if (channel == m_Channel[voice])
            {
                if(m_ChannelCount[channel] == 1)
                {
                    // mono mode
                    if (IsNoteOn(message))
                    {
                        if(0== bitRead(m_IsActive[voice], 0))
                        {
                            m_MidiNote[voice] = midiNote;
                            bitSet(m_IsActive[voice], 0);
                            // update pitch out and gate out
                            VoiceOn(voice, m_ChannelBaseNote[channel], m_MidiNote[voice], velocity);
                            handled = true;
                        }
                        else if(0 ==bitRead(m_IsActive[voice], 1))
                        {
                            m_MidiNote2[voice] = midiNote;
                            bitSet(m_IsActive[voice], 1);
                            // update pitch out and gate out
                            VoiceOn(voice, m_ChannelBaseNote[channel], m_MidiNote2[voice], velocity);
                            handled = true;
                        }
                        // more than 2 notes pressed => drop note
                    }
                    else if (IsNoteOff(message))
                    {
                        if(midiNote == m_MidiNote[voice])
                        {
                            bitClear(m_IsActive[voice], 0); // 1st note off

                            if(0 == m_IsActive[voice])
                            {
                                // 2nd note is also off => 
                                // keep midi note
                                // update gate out
                                VoiceOff(voice);
                                handled = true;
                            }
                            else
                            {
                                // 2nd note is on => 
                                // change pitch to 2nd note, keep gate on, keep velocity
                                VoiceOn(voice, m_ChannelBaseNote[channel], m_MidiNote2[voice]);
                                handled = true;
                            }
                        }
                        else if(midiNote == m_MidiNote2[voice])
                        {
                            bitClear(m_IsActive[voice], 1); // 2nd note off

                            if(0 == m_IsActive[voice])
                            {
                                // 1st note is also off => 
                                // keep midi note
                                // update gate out
                                VoiceOff(voice);
                                handled = true;
                            }
                            else
                            {
                                // 1st note is on => 
                                // change pitch to 1st note, keep gate on, keep velocity
                                VoiceOn(voice, m_ChannelBaseNote[channel], m_MidiNote[voice]);
                                handled = true;
                            }
                        }
                    }
                }
                else
                {
                    // poly mode
                    if (IsNoteOn(message) && m_IsActive[voice] == 0)
                    {
                        m_MidiNote[voice] = midiNote;
                        m_IsActive[voice] = 1;
                        // update pitch out and gate out
                        VoiceOn(voice,  m_ChannelBaseNote[channel], m_MidiNote[voice], velocity);
                        handled = true;
                    }
                    else if (IsNoteOff(message) && m_IsActive[voice] == 1 && midiNote == m_MidiNote[voice])
                    {
                        m_IsActive[voice] = 0;
                        // keep midi note and velocity
                        // update gate out
                        VoiceOff(voice);
                        handled = true;
                    }
                }
            }
        }
    }
}

void Midi2PGVG::ToggleLearning()
{
    if (IsLearning())
    {
        // no learning
        m_LearnIndex = -1;

        // statusled on
        m_LedOut->LedOn();
    }
    else
    {
        // start learning
        m_LearnIndex = 0;

        // all gates off here?
        for (int voice = 0; voice < NumVoices; ++voice)
        {
            m_IsActive[voice] = 0;
            VoiceOff(voice);
        }

        // blink statusled
        m_LedOut->LedBlink();
    }
}

bool Midi2PGVG::IsLearning() const
{
    return 0 <= m_LearnIndex;
}

void Midi2PGVG::PrintState()
{
    Serial.println(m_LearnIndex, DEC);

    for(int ch = 0; ch<NumMidiChannels; ++ch)
    {
        if(0<m_ChannelCount[ch])
        {
            Serial.print(ch, HEX);
            Serial.print(' ');
            Serial.print(m_ChannelCount[ch], DEC);
            Serial.print(' ');
            Serial.println(m_ChannelBaseNote[ch], HEX);    
        }
    }
    Serial.println();

    for (int idx = 0; idx < NumVoices; ++idx)
    {
        Serial.print(m_Channel[idx], HEX);
        Serial.print(' ');
        Serial.print(m_IsActive[idx], DEC);
        Serial.print(' ');
        Serial.print(m_ChannelBaseNote[m_Channel[idx]], HEX);
        Serial.print(' ');
        Serial.print(m_MidiNote[idx], HEX);
        Serial.print(' ');
        Serial.println(m_MidiNote2[idx], HEX);
    }
}

void Midi2PGVG::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'P');
    EEPROM.update(off++, 'V');
    for(int voice = 0; voice<NumVoices; ++voice)
    {
        uint8_t channel = m_Channel[voice];
        uint8_t baseNote = m_ChannelBaseNote[channel];
        EEPROM.update(off++, channel);
        EEPROM.update(off++, baseNote);
    }
}

int Midi2PGVG::paramSize() const
{
    return 2+2*NumVoices;//actually 10
}

void Midi2PGVG::loadParams(int offset)
{
    int off = offset;
    if ('P' == EEPROM.read(off++) && 'V' == EEPROM.read(off++))
    {
        // channel count to zero for all channels???
        for(int ch = 0; ch<NumMidiChannels; ++ch)
        {
            m_ChannelCount[ch] = 0;
        }
        for(int voice = 0; voice<NumVoices; ++voice)
        {
            uint8_t channel = EEPROM.read(off++);
            uint8_t baseNote = EEPROM.read(off++);
            m_Channel[voice] = channel;
            ++m_ChannelCount[channel];
            m_ChannelBaseNote[channel] = baseNote;
        }
    }
}