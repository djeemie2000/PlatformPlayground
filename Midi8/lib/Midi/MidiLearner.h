#pragma once
#include <Arduino.h>

class MidiLearner
{
public:
    MidiLearner(uint8_t midiChannel, uint8_t midiNote);

    void StartMidiLearn()
    {
        m_LearnChannel = true;
        m_LearnNote = true;
        for (int idx = 0; idx < NumCC; ++idx)
        {
            m_LearnCC[idx] = true;
        }
    }

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
    {
        if (CheckChannel(Channel))
        {
            if (m_LearnNote)
            {
                m_MidiNote = MidiNote;
                m_LearnNote = false;
            }
        }
    }

    void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
    {
        if (CheckChannel(Channel))
        {
            for (int idx = 0; idx < NumCC; ++idx)
            {
                if (m_LearnCC[idx])
                {
                    m_MidiCC[idx] = Controller;
                    m_LearnCC[idx] = false;
                    break;
                }
            }
        }
    }

    bool CheckChannel(uint8_t midiChannel)
    {
        if (m_LearnChannel)
        {
            m_MidiChannel = midiChannel;
            m_LearnChannel = false;
        }
        return m_MidiChannel == midiChannel;
    }

    uint8_t m_MidiChannel;
    bool m_LearnChannel;
    uint8_t m_MidiNote;
    bool m_LearnNote;
    static const int NumCC = 2;
    uint8_t m_MidiCC[NumCC];
    bool m_LearnCC[NumCC];
};
