#pragma once
#include <Arduino.h>

class MidiVoiceMessage;
template<int Size>
class GateOutBank;
class LedOut;
template<int Size>
class CVOutBank;

template<int NumVoices>
class Midi2PG
{
public:
    Midi2PG();

    void Begin(GateOutBank<NumVoices>* gates, LedOut* ledOut, CVOutBank<NumVoices>* cvOuts);

    void OnMessage(MidiVoiceMessage &message);
    void OnTick(uint8_t counter); // only for blinking in learn mode?
    void ToggleLearning();
    bool IsLearning() const;

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

    void PrintState();

private:
    void VoiceOn(int voice, uint8_t midiBaseNote, uint8_t midiNote, uint8_t velocity);
    void VoiceOn(int voice, uint8_t midiBaseNote, uint8_t midiNote);
    void VoiceOff(int voice);

    static const int NumMidiChannels = 16;

    GateOutBank<NumVoices>* m_Gates;
    LedOut* m_LedOut;
    CVOutBank<NumVoices>* m_CvOuts;
    
    uint8_t m_Channel[NumVoices];
    uint8_t m_MidiNote[NumVoices];
    uint8_t m_MidiNote2[NumVoices];//basic 2 note stack for mono voice
    uint8_t m_IsActive[NumVoices];//bit0 bit1

    uint8_t m_ChannelCount[NumMidiChannels]; // mono or poly?
    uint8_t m_ChannelBaseNote[NumMidiChannels]; // 

    int m_LearnIndex;
};

#include "midi2pg.h"
#include "ledout.h"
#include "gateoutbank.h"
#include "cvoutbank.h"
#include "EEPROM.h"
#include "midivoicemessage.h"

template<int NumVoices>
Midi2PG<NumVoices>::Midi2PG()
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

template<int NumVoices>
void Midi2PG<NumVoices>::Begin(GateOutBank<NumVoices> *gates, LedOut *ledOut, CVOutBank<NumVoices> *cvOuts)
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
        m_Gates->GateOff(voice);
        m_CvOuts->PitchOut(voice, 0x00, 0x00);
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

template<int NumVoices>
void Midi2PG<NumVoices>::VoiceOn(int voice, uint8_t midiBaseNote, uint8_t midiNote, uint8_t velocity)
{
    m_Gates->GateOn(voice);
    m_CvOuts->PitchOut(voice, midiBaseNote, midiNote);
    // ignore velocity
}

template<int NumVoices>
void Midi2PG<NumVoices>::VoiceOn(int voice, uint8_t midiBaseNote, uint8_t midiNote)
{
    m_Gates->GateOn(voice);
    m_CvOuts->PitchOut(voice, midiBaseNote, midiNote);
}

template<int NumVoices>
void Midi2PG<NumVoices>::VoiceOff(int voice)
{
    // keep midi note, keep velocity
    // update gate out
    m_Gates->GateOff(voice);
}

template<int NumVoices>
void Midi2PG<NumVoices>::OnMessage(MidiVoiceMessage &message)
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
                            VoiceOn(voice, m_ChannelBaseNote[channel], m_MidiNote[voice]);
                            handled = true;
                        }
                        else if(0 ==bitRead(m_IsActive[voice], 1))
                        {
                            m_MidiNote2[voice] = midiNote;
                            bitSet(m_IsActive[voice], 1);
                            // update pitch out and gate out
                            VoiceOn(voice, m_ChannelBaseNote[channel], m_MidiNote2[voice]);
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
                                // change pitch to 2nd note, keep gate on
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
                                // change pitch to 1st note, keep gate on
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
                        VoiceOn(voice, m_ChannelBaseNote[channel], m_MidiNote[voice]);
                        handled = true;
                    }
                    else if (IsNoteOff(message) && m_IsActive[voice] == 1 && midiNote == m_MidiNote[voice])
                    {
                        m_IsActive[voice] = 0;
                        // keep midi note
                        // update gate out
                        VoiceOff(voice);
                        handled = true;
                    }

                }
            }
        }
    }
}

template<int NumVoices>
void Midi2PG<NumVoices>::ToggleLearning()
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

template<int NumVoices>
bool Midi2PG<NumVoices>::IsLearning() const
{
    return 0 <= m_LearnIndex;
}

template<int NumVoices>
void Midi2PG<NumVoices>::PrintState()
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

template<int NumVoices>
void Midi2PG<NumVoices>::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'P');
    EEPROM.update(off++, 'G');
    for(int voice = 0; voice<NumVoices; ++voice)
    {
        uint8_t channel = m_Channel[voice];
        uint8_t baseNote = m_ChannelBaseNote[channel];
        EEPROM.update(off++, channel);
        EEPROM.update(off++, baseNote);
    }
}

template<int NumVoices>
int Midi2PG<NumVoices>::paramSize() const
{
    return 2+2*NumVoices;
}

template<int NumVoices>
void Midi2PG<NumVoices>::loadParams(int offset)
{
    int off = offset;
    if ('P' == EEPROM.read(off++) && 'G' == EEPROM.read(off++))
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
