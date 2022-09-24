#pragma once
#include <Arduino.h>

#include "midivoicemessage.h"

class MidiVoiceMessage;

class Midi2PG
{
public:
    static const int NumVoices = 8;

    Midi2PG();

    void Begin(uint8_t statusLed);

    void OnMessage(MidiVoiceMessage &message);
    void OnTick(uint8_t counter); // only for blinking in learn mode?
    void ToggleLearning();
    bool IsLearning() const;

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

    void PrintState();

private:
    uint8_t m_Channel[NumVoices];
    uint8_t m_MidiBaseNote[NumVoices];
    uint8_t m_MidiNote[NumVoices];
    uint8_t m_IsActive[NumVoices];

    int m_LearnIndex;
    
    uint8_t m_StatusLed;
};
