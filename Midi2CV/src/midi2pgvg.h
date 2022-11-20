#pragma once
#include <Arduino.h>

#include "midivoicemessage.h"

class MidiVoiceMessage;
class GateOutBank;
class LedOut;
class CVOutBank;

class Midi2PGVG
{
public:
    static const int NumVoices = 4;

    Midi2PGVG();

    void Begin(GateOutBank* gates, LedOut* ledOut, CVOutBank* cvOuts);

    void OnMessage(MidiVoiceMessage &message);
    void OnTick(uint8_t counter); // only for blinking in learn mode?
    void ToggleLearning();
    bool IsLearning() const;

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

    void PrintState();

private:
    static const int NumMidiChannels = 16;

    GateOutBank* m_Gates;
    LedOut* m_LedOut;
    CVOutBank* m_CvOuts;
    uint8_t m_Channel[NumVoices];
    uint8_t m_MidiNote[NumVoices];
    uint8_t m_IsActive[NumVoices];

    uint8_t m_ChannelCount[NumMidiChannels]; // mono or poly?
    uint8_t m_ChannelBaseNote[NumMidiChannels]; // 

    int m_LearnIndex;
};
