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
    void VoiceOn(int voice, uint8_t midiBaseNote, uint8_t midiNote, uint8_t velocity);
    void VoiceOn(int voice, uint8_t midiBaseNote, uint8_t midiNote);
    void VoiceOff(int voice);

    static const int NumMidiChannels = 16;

    GateOutBank* m_Gates;
    LedOut* m_LedOut;
    CVOutBank* m_CvOuts;
    uint8_t m_Channel[NumVoices];
    uint8_t m_MidiNote[NumVoices];
    uint8_t m_MidiNote2[NumVoices];//basic 2 note stack for mono voice
    uint8_t m_IsActive[NumVoices];//bit0 bit1

    uint8_t m_ChannelCount[NumMidiChannels]; // mono or poly?
    uint8_t m_ChannelBaseNote[NumMidiChannels]; // 

    int m_LearnIndex;
};
