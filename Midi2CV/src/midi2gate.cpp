#include "midi2gate.h"

Midi2Gate::Midi2Gate()
    : m_LearnIndex(-1)
{
    for (int gate = 0; gate < NumGates; ++gate)
    {
        m_OutputPin[gate] = -1;
        m_Channel[gate] = 0x00;
        m_MidiNote[gate] = 0x00;
        m_Gate[gate] = 0;
    }
}

void Midi2Gate::Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3,
                      uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7,
                      uint8_t pin8, uint8_t pin9, uint8_t pin10, uint8_t pin11)
{
    m_OutputPin[0] = pin0;
    m_OutputPin[1] = pin1;
    m_OutputPin[2] = pin2;
    m_OutputPin[3] = pin3;

    m_OutputPin[4] = pin4;
    m_OutputPin[5] = pin5;
    m_OutputPin[6] = pin6;
    m_OutputPin[7] = pin7;

    m_OutputPin[8] = pin8;
    m_OutputPin[9] = pin9;
    m_OutputPin[10] = pin10;
    m_OutputPin[11] = pin11;

    for(int gate = 0; gate<NumGates; ++gate)
    {
        if(0<=m_OutputPin[gate])
        {
            pinMode(m_OutputPin[gate], OUTPUT);
        }
        m_Gate[gate] = 0;
    }
}

void Midi2Gate::OnMessage(MidiVoiceMessage &message)
{
    // TODO handle notes/gates that are on when starting to learn
    if (IsLearning())
    {
        // learning
        if (IsNoteOn(message))
        {
            m_Channel[m_LearnIndex] = Channel(message);
            m_MidiNote[m_LearnIndex] = MidiNote(message);

            ++m_LearnIndex;
            if (NumGates <= m_LearnIndex)
            {
                // all gates learned
                m_LearnIndex = -1;
            }
        }
        // ignore note off
    }
    else
    {
        // normal operation
        uint8_t channel = Channel(message);
        uint8_t midiNote = MidiNote(message);
        for (int gate = 0; gate < NumGates; ++gate)
        {
            if (channel == m_Channel[gate] && midiNote == m_MidiNote[gate])
            {
                if (IsNoteOn(message))
                {
                    m_Gate[gate] = 1;
                }
                else if (IsNoteOff(message))
                {
                    m_Gate[gate] = 0;
                }
                // TODO update here or in onTick() ???
                if (0 <= m_OutputPin[gate])
                {
                    digitalWrite(m_OutputPin[gate], m_Gate[gate]);
                }
            }
        }
    }
}

void Midi2Gate::OnTick(uint8_t counter)
{
    // only for blinking in learn mode?
    // TODO counter for blinking
    if (IsLearning())
    {
        bool blink = counter & 0x40;
        for (int gate = 0; gate < NumGates; ++gate)
        {
            m_Gate[gate] = (m_LearnIndex == gate && blink) ? 1 : 0;
            digitalWrite(m_OutputPin[gate], m_Gate[gate]);//TODO fastdigitalwrite
        }
    }
}

void Midi2Gate::ToggleLearning()
{
    if (IsLearning())
    {
        // no learning
        m_LearnIndex = -1;
    }
    else
    {
        // start learning
        m_LearnIndex = 0;
        
        // all gates off here?
        for(int gate = 0; gate<NumGates; ++ gate)
        {
            m_Gate[gate] = 0;
            digitalWrite(m_OutputPin[gate], m_Gate[gate]);//TODO fastdigitalwrite
        }
    }
}

bool Midi2Gate::IsLearning() const
{
    return 0 <= m_LearnIndex;
}
