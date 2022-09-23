#pragma once
#include <Arduino.h>

#include "midivoicemessage.h"

class MidiVoiceMessage;

class Midi2Gate
{
public:
    static const int NumGates = 8;

    Midi2Gate();

    void Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, 
                uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7,
                uint8_t statusLed);

    void OnMessage(MidiVoiceMessage &message);
    void OnTick(uint8_t counter); // only for blinking in learn mode?
    void ToggleLearning();
    bool IsLearning() const;

private:
    uint8_t m_OutputPin[NumGates];
    uint8_t m_Channel[NumGates];
    uint8_t m_MidiNote[NumGates];
    uint8_t m_Gate[NumGates];
    int m_LearnIndex;
    
    uint8_t m_StatusLed;
};
