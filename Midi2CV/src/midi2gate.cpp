#include "midi2gate.h"
#include "midivoicemessage.h"
#include "gateoutbank.h"
#include "ledout.h"
#include "EEPROM.h"

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

void Midi2Gate::Begin(GateOutBank<Midi2Gate::NumGates>* gates, LedOut* ledOut)
{
    m_Gates = gates;
    m_LedOut = ledOut;

    for(int gate = 0; gate<NumGates; ++gate)
    {       
        m_Gate[gate] = 0;
        m_Gates->GateOff(gate);
    }

    // status led on
    m_LedOut->LedOn();
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

                m_LedOut->LedOn();
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

void Midi2Gate::ToggleLearning()
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
        for(int gate = 0; gate<NumGates; ++ gate)
        {
            m_Gate[gate] = 0;
            m_Gates->GateOff(gate);
        }

        // blink statusled
        m_LedOut->LedBlink();
    }
}

bool Midi2Gate::IsLearning() const
{
    return 0 <= m_LearnIndex;
}

void Midi2Gate::PrintState()
{
    Serial.println(m_LearnIndex, DEC);
    for(int idx = 0; idx<Midi2Gate::NumGates;++idx)
    {
        Serial.print(m_Channel[idx], HEX);
        Serial.print(' ');
        Serial.print(m_MidiNote[idx], HEX);
        Serial.print(' ');
        Serial.println(m_Gate[idx], DEC);
    }
}

void Midi2Gate::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'G');
    EEPROM.update(off++, 'G');
    for(int voice = 0; voice<NumGates; ++voice)
    {
        uint8_t channel = m_Channel[voice];
        uint8_t baseNote = m_MidiNote[voice];
        EEPROM.update(off++, channel);
        EEPROM.update(off++, baseNote);
    }
}

int Midi2Gate::paramSize() const
{
    return 2+2*NumGates;
}

void Midi2Gate::loadParams(int offset)
{
    int off = offset;
    if ('G' == EEPROM.read(off++) && 'G' == EEPROM.read(off++))
    {
        for(int voice = 0; voice<NumGates; ++voice)
        {
            uint8_t channel = EEPROM.read(off++);
            uint8_t baseNote = EEPROM.read(off++);
            m_Channel[voice] = channel;
            m_MidiNote[voice] = baseNote;
        }
    }
}