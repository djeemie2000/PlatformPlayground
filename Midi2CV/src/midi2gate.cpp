#include "midi2gate.h"
#include "midivoicemessage.h"
#include "gateoutbank.h"

Midi2Gate::Midi2Gate()
    : m_Gates(0)
    , m_LearnIndex(-1)
{
    for (int gate = 0; gate < NumGates; ++gate)
    {
        m_Channel[gate] = 0x00;
        m_MidiNote[gate] = 0x00;
        m_Gate[gate] = 0;
    }
}

void Midi2Gate::Begin(GateOutBank* gates)
{
    m_Gates = gates;

    for(int gate = 0; gate<NumGates; ++gate)
    {       
        m_Gate[gate] = 0;
        m_Gates->GateOff(gate);
    }

    // status led on
    m_Gates->GateOn(8);
}

void Midi2Gate::OnMessage(MidiVoiceMessage &message)
{
    // handle notes/gates that are on when starting to learn -> in ToggleLearning()
    if (IsLearning())
    {
        // learning
        if (IsNoteOn(message))
        {
            m_Channel[m_LearnIndex] = Channel(message);
            m_MidiNote[m_LearnIndex] = MidiNote(message);

            // gate high ??

            ++m_LearnIndex;
            if (NumGates <= m_LearnIndex)
            {
                // all gates learned
                m_LearnIndex = -1;
            }
        }
        // note off => gate low
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
                    m_Gates->GateOn(gate);
                }
                else if (IsNoteOff(message))
                {
                    m_Gate[gate] = 0;
                    m_Gates->GateOff(gate);
                }
            }
        }
    }
}

void Midi2Gate::OnTick(uint8_t counter)
{
    // only for blinking led in learn mode?
    // TODO counter for blinking
    if (IsLearning())
    {
        bool blink = counter & 0x40;
        SetGate(*m_Gates, 8, blink ? 1 : 0);
    }
    else
    {
        m_Gates->GateOn(8);
    }
}

void Midi2Gate::ToggleLearning()
{
    if (IsLearning())
    {
        // no learning
        m_LearnIndex = -1;

        // statusled here?
         m_Gates->GateOn(8);
    }
    else
    {
        // start learning
        m_LearnIndex = 0;
        
        // all gates off here?
        for(int gate = 0; gate<NumGates; ++ gate)
        {
            m_Gate[gate] = 0;
            m_Gates->GateOff(gate);
        }
    }
}

bool Midi2Gate::IsLearning() const
{
    return 0 <= m_LearnIndex;
}

void Midi2Gate::PrintState()
{
    Serial.println(m_LearnIndex, DEC);
    for(int idx = 0; idx<NumGates;++idx)
    {
        Serial.print(m_Channel[idx], HEX);
        Serial.print(' ');
        Serial.print(m_MidiNote[idx], HEX);
        Serial.print(' ');
        Serial.println(m_Gate[idx], DEC);
    }
}
