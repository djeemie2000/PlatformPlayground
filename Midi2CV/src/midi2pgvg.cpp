#include "midi2pgvg.h"
#include "ledout.h"
#include "gateoutbank.h"
#include "cvoutbank.h"

Midi2PGVG::Midi2PGVG()
    : m_LearnIndex(-1)
{
    for (int voice = 0; voice < NumVoices; ++voice)
    {        
        m_Channel[voice] = 0x00;
        m_MidiBaseNote[voice] = 0x00;
        m_MidiNote[voice] = 0x00;
        m_IsActive[voice] = 0;
    }
}

void Midi2PGVG::Begin(GateOutBank* gates, LedOut* ledOut, CVOutBank* cvOuts)
{
    m_Gates = gates;
    m_LedOut = ledOut;
    m_CvOuts = cvOuts;

    for(int voice = 0; voice<NumVoices; ++voice)
    {       
        m_Channel[voice] = 0x00;
        m_MidiBaseNote[voice] = 0x00;
        m_MidiNote[voice] = 0x00;
        m_IsActive[voice] = 0;
        m_Gates->GateOff(voice*2);
        m_Gates->GateOff(voice*2+1);
        m_CvOuts->PitchOut(voice*2, 0x00, 0x00);
        m_CvOuts->VelocityOut(voice*2+1, 0x00);
    }

    // status led on
    m_LedOut->LedOn();
}

void Midi2PGVG::OnMessage(MidiVoiceMessage &message)
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
        uint8_t velocity = Velocity(message);
        bool handled = false;
        for (int voice = 0; voice < NumVoices && !handled; ++voice)
        {
            if (channel == m_Channel[voice])
            {
                if (IsNoteOn(message) && m_IsActive[voice] == 0)
                {
                    m_MidiNote[voice] = midiNote;
                    m_IsActive[voice] = 1;
                    // update pitch out and gate out
                    m_Gates->GateOn(voice*2);
                    m_Gates->GateOn(voice*2+1);
                    m_CvOuts->PitchOut(voice*2, m_MidiBaseNote[voice], m_MidiNote[voice]);
                    m_CvOuts->VelocityOut(voice*2+1, velocity);
                    handled = true;
                }
                else if (IsNoteOff(message) && m_IsActive[voice] == 1)
                {
                    m_IsActive[voice] = 0;
                    // keep midi note and velocity
                    // update gate out
                    m_Gates->GateOff(voice*2);
                    m_Gates->GateOff(voice*2+1);
                    handled = true;
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

        // statusled here?
        m_LedOut->LedBlink();
    }
    else
    {
        // start learning
        m_LearnIndex = 0;
        
        // all gates off here?
        for(int voice = 0; voice<NumVoices; ++voice)
        {
            m_IsActive[voice] = 0;
            m_Gates->GateOff(voice*2);
            m_Gates->GateOff(voice*2+1);
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
    for(int idx = 0; idx<NumVoices;++idx)
    {
        Serial.print(m_Channel[idx], HEX);
        Serial.print(' ');
        Serial.print(m_MidiBaseNote[idx], HEX);
        Serial.print(' ');
        Serial.print(m_MidiNote[idx], HEX);
        Serial.print(' ');
        Serial.println(m_IsActive[idx], DEC);
    }
}
