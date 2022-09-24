#include "midi2pg.h"

Midi2PG::Midi2PG()
    : m_LearnIndex(-1)
{
    for (int gate = 0; gate < NumVoices; ++gate)
    {        
        m_Channel[gate] = 0x00;
        m_MidiBaseNote[gate] = 0x00;
        m_MidiNote[gate] = 0x00;
        m_IsActive[gate] = 0;
    }
}

void Midi2PG::Begin(uint8_t statusLed)
{
    m_StatusLed = statusLed;

    pinMode(m_StatusLed, OUTPUT);
    digitalWrite(m_StatusLed, HIGH);
}

void Midi2PG::OnMessage(MidiVoiceMessage &message)
{
    if(IsLearning())
    {
        m_Channel[m_LearnIndex] = Channel(message);
        m_MidiBaseNote[m_LearnIndex] = MidiNote(message);

        // if other voices have same channel, use same (first) base note
        for(int voice = 0; voice<m_LearnIndex; ++voice)
        {
            if(m_Channel[m_LearnIndex] == m_Channel[voice])
            {
                m_MidiBaseNote[m_LearnIndex] = m_MidiBaseNote[voice];
                break;
            }
        }

        ++m_LearnIndex;
        if (NumVoices <= m_LearnIndex)
        {
            // all gates learned
            m_LearnIndex = -1;
        }

    }
    else 
    {
        // normal operation (poly mode)
        // first voice matching the channel that is not active yet will handle a note on
        // note off => handled by active voice when matching channel and note
        uint8_t channel = Channel(message);
        uint8_t midiNote = MidiNote(message);
        bool handled = false;
        for (int voice = 0; voice < NumVoices && !handled; ++voice)
        {
            if (channel == m_Channel[voice])
            {
                if (IsNoteOn(message) && m_IsActive[voice] == 0)
                {
                    m_MidiNote[voice] = midiNote;
                    m_IsActive[voice] = 1;
                    //TODO update pitch out and gate out
                    handled = true;
                }
                else if (IsNoteOff(message) && m_IsActive[voice] == 1)
                {
                    m_IsActive[voice] = 0;
                    // keep midi note
                    //TODO update gate out
                    handled = true;
                }                
            }
        }
    }
}

void Midi2PG::OnTick(uint8_t counter)
{
    // only for blinking in learn mode?
    // TODO counter for blinking
    if (IsLearning())
    {
        bool blink = counter & 0x40;
        digitalWrite(m_StatusLed, blink ? 1 : 0);
    }
    else
    {
        digitalWrite(m_StatusLed, HIGH);
    }

    //TODO update gates here???
}

void Midi2PG::ToggleLearning()
{
    if (IsLearning())
    {
        // no learning
        m_LearnIndex = -1;

        // statusled here?
        digitalWrite(m_StatusLed, HIGH);
    }
    else
    {
        // start learning
        m_LearnIndex = 0;
        
        // all gates off here?
        for(int gate = 0; gate<NumVoices; ++ gate)
        {
            m_IsActive[gate] = 0;
            // TODO all gate off
            // digitalWrite(m_OutputPin[gate], m_Gate[gate]);//TODO fastdigitalwrite
        }
    }
}

bool Midi2PG::IsLearning() const
{
    return 0 <= m_LearnIndex;
}
