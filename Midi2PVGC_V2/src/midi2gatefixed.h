#pragma once
#include <Arduino.h>

class MidiVoiceMessage;
template<int NumGates>
class GateOutBank;
class LedOut;

template<int NumGates>
class Midi2GateFixed
{
public:
    Midi2GateFixed();

    void Begin(GateOutBank<NumGates>* gates, LedOut* ledOut);
    void Assign(int idx, uint8_t channel, uint8_t midiNote);

    void OnMessage(MidiVoiceMessage &message);

    void PrintState();

private:
    GateOutBank<NumGates>* m_Gates;
    LedOut* m_LedOut;
    uint8_t m_Channel[NumGates];
    uint8_t m_MidiNote[NumGates];
    uint8_t m_Gate[NumGates];
};

#include "midivoicemessage.h"
#include "gateoutbank.h"
#include "ledout.h"
#include "EEPROM.h"

template<int NumGates>
Midi2GateFixed<NumGates>::Midi2GateFixed()
    : m_Gates(0)
    , m_LedOut(0)
{
    for (int gate = 0; gate < NumGates; ++gate)
    {
        m_Channel[gate] = 0x00;
        m_MidiNote[gate] = 0x00;
        m_Gate[gate] = 0;
    }
}

template<int NumGates>
void Midi2GateFixed<NumGates>::Begin(GateOutBank<NumGates>* gates, LedOut* ledOut)
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

template<int NumGates>
void Midi2GateFixed<NumGates>::Assign(int idx, uint8_t channel, uint8_t midiNote)
{
    if(0<=idx && idx<NumGates)
    {
        m_Channel[idx] = channel;
        m_MidiNote[idx] = midiNote;
    }
}


template<int NumGates>
void Midi2GateFixed<NumGates>::OnMessage(MidiVoiceMessage &message)
{
    // handle notes/gates that are on when starting to learn -> in ToggleLearning()
    
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


template<int NumGates>
void Midi2GateFixed<NumGates>::PrintState()
{
    for(int idx = 0; idx<NumGates;++idx)
    {
        Serial.print(m_Channel[idx], HEX);
        Serial.print(' ');
        Serial.print(m_MidiNote[idx], HEX);
        Serial.print(' ');
        Serial.println(m_Gate[idx], DEC);
    }
}


